package com.example.r6controller

import android.content.Context
import android.location.Location
import android.net.ConnectivityManager
import android.net.Network
import android.net.NetworkCapabilities
import android.net.NetworkRequest
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.Response
import okhttp3.WebSocket
import okhttp3.WebSocketListener
import org.json.JSONObject
import java.util.concurrent.TimeUnit
import kotlin.math.abs

data class GpsData(
    val fix: Boolean,
    val lat: Double,
    val lon: Double,
    val sats: Int,
    val speedKmh: Double,
)

data class LatLon(val lat: Double, val lon: Double)

enum class LinkState { SEARCHING_WIFI, CONNECTING, CONNECTED }

/**
 * Conexión única con el carro, compartida por las dos pantallas.
 *
 * Todo el tráfico hacia el carro se fuerza por la red WiFi "R6Controller"
 * (que no tiene internet), así el resto del teléfono —incluido el mapa—
 * puede seguir usando los datos móviles.
 */
object CarLink {
    const val WS_URL = "ws://192.168.4.1/ws"
    const val CAM_URL = "http://192.168.4.2:81/stream"
    private const val SEND_INTERVAL_MS = 80L

    // ---- Corrección de motores (sin reprogramar la ESP32) ----
    private const val INVERT_LEFT_MOTOR  = false  // motor conectado a AO1/AO2 (canal A)
    private const val INVERT_RIGHT_MOTOR = true   // motor conectado a BO1/BO2 (canal B)
    private const val SWAP_SIDES         = false  // true si al girar a la derecha va a la izquierda

    // ---- Límite de potencia del joystick ----
    private const val MAX_POWER     = 70   // % de potencia con el joystick al máximo (1..100)
    private const val TURN_STRENGTH = 60   // % de fuerza del giro (1..100). Menos = gira más suave

    private val scope = CoroutineScope(SupervisorJob() + Dispatchers.IO)

    private val _state = MutableStateFlow(LinkState.SEARCHING_WIFI)
    val state: StateFlow<LinkState> = _state.asStateFlow()

    private val _gps = MutableStateFlow<GpsData?>(null)
    val gps: StateFlow<GpsData?> = _gps.asStateFlow()

    private val _track = MutableStateFlow<List<LatLon>>(emptyList())
    val track: StateFlow<List<LatLon>> = _track.asStateFlow()

    @Volatile private var client: OkHttpClient? = null
    @Volatile private var ws: WebSocket? = null
    @Volatile private var joyX = 0
    @Volatile private var joyY = 0
    private var started = false

    fun start(context: Context) {
        if (started) return
        started = true

        val cm = context.applicationContext.getSystemService(ConnectivityManager::class.java)
        val request = NetworkRequest.Builder()
            .addTransportType(NetworkCapabilities.TRANSPORT_WIFI)
            .removeCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
            .build()

        cm.requestNetwork(request, object : ConnectivityManager.NetworkCallback() {
            override fun onAvailable(network: Network) {
                client = OkHttpClient.Builder()
                    .socketFactory(network.socketFactory)   // <- usa solo el WiFi del carro
                    .connectTimeout(3, TimeUnit.SECONDS)
                    .pingInterval(2, TimeUnit.SECONDS)
                    .build()
                connectWs()
            }

            override fun onLost(network: Network) {
                client = null
                ws?.cancel()
                ws = null
                _state.value = LinkState.SEARCHING_WIFI
            }
        })

        // Envía el joystick continuamente: también sirve de "latido" para el failsafe
        scope.launch {
            while (isActive) {
                ws?.send("J,$joyX,$joyY")
                delay(SEND_INTERVAL_MS)
            }
        }
    }

    fun setJoystick(x: Int, y: Int) {
        // 1. Lo que queremos que haga cada rueda (misma fórmula que la ESP32)
        val turn = x * TURN_STRENGTH / 100
        var left = y + turn
        var right = y - turn
        val biggest = maxOf(abs(left), abs(right))
        if (biggest > 100) {
            left = left * 100 / biggest
            right = right * 100 / biggest
        }

        // Límite de potencia
        left = left * MAX_POWER / 100
        right = right * MAX_POWER / 100

        // 2. Correcciones
        if (SWAP_SIDES) { val t = left; left = right; right = t }
        if (INVERT_LEFT_MOTOR) left = -left
        if (INVERT_RIGHT_MOTOR) right = -right

        // 3. Convertir de vuelta a x,y para que la ESP32, al hacer
        //    A = y + x  y  B = y - x, obtenga exactamente left y right
        joyX = (left - right) / 2
        joyY = (left + right) / 2
    }

    /** Cliente para el video (lectura con timeout para detectar cortes). */
    fun streamClient(): OkHttpClient? =
        client?.newBuilder()?.readTimeout(5, TimeUnit.SECONDS)?.build()

    private fun connectWs() {
        val c = client ?: return
        _state.value = LinkState.CONNECTING
        ws = c.newWebSocket(Request.Builder().url(WS_URL).build(), object : WebSocketListener() {
            override fun onOpen(webSocket: WebSocket, response: Response) {
                _state.value = LinkState.CONNECTED
            }

            override fun onMessage(webSocket: WebSocket, text: String) {
                handleMessage(text)
            }

            override fun onFailure(webSocket: WebSocket, t: Throwable, response: Response?) {
                handleDrop(webSocket)
            }

            override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
                handleDrop(webSocket)
            }
        })
    }

    private fun handleDrop(socket: WebSocket) {
        if (ws !== socket) return
        ws = null
        if (client == null) return
        _state.value = LinkState.CONNECTING
        scope.launch {
            delay(1500)
            if (ws == null && client != null) connectWs()
        }
    }

    private fun handleMessage(text: String) {
        val o = runCatching { JSONObject(text) }.getOrNull() ?: return
        if (o.optString("t") != "gps") return
        val g = GpsData(
            fix = o.optBoolean("fix"),
            lat = o.optDouble("lat"),
            lon = o.optDouble("lon"),
            sats = o.optInt("sat"),
            speedKmh = o.optDouble("spd"),
        )
        _gps.value = g
        if (g.fix) addTrackPoint(LatLon(g.lat, g.lon))
    }

    private fun addTrackPoint(p: LatLon) {
        val current = _track.value
        val last = current.lastOrNull()
        if (last != null) {
            val d = FloatArray(1)
            Location.distanceBetween(last.lat, last.lon, p.lat, p.lon, d)
            if (d[0] < 1.5f) return   // ignora ruido del GPS
        }
        _track.value = (current + p).takeLast(2000)
    }
}
