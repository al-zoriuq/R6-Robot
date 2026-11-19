package com.example.r6controller

import android.graphics.Color
import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.lifecycleScope
import androidx.lifecycle.repeatOnLifecycle
import kotlinx.coroutines.launch
import org.osmdroid.config.Configuration
import org.osmdroid.tileprovider.tilesource.TileSourceFactory
import org.osmdroid.util.GeoPoint
import org.osmdroid.views.MapView
import org.osmdroid.views.overlay.Marker
import org.osmdroid.views.overlay.Polyline
import java.util.Locale

/** Pantalla de mapa: posición del carro + recorrido + botón "Regresar". */
class MapActivity : AppCompatActivity() {

    private lateinit var map: MapView
    private lateinit var marker: Marker
    private lateinit var trail: Polyline
    private lateinit var info: TextView
    private var centeredOnce = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        Configuration.getInstance().userAgentValue = "R6Controller/1.0 (Android; carro RC)"   // OpenStreetMap bloquea "com.example..."
        setContentView(R.layout.activity_map)
        hideSystemBars()

        map = findViewById(R.id.mapView)
        info = findViewById(R.id.gpsInfo)

        map.setTileSource(TileSourceFactory.MAPNIK)
        map.setMultiTouchControls(true)
        map.controller.setZoom(18.0)
        map.controller.setCenter(GeoPoint(4.6097, -74.0817))   // posición inicial cualquiera

        trail = Polyline(map).apply {
            outlinePaint.color = Color.RED
            outlinePaint.strokeWidth = 8f
        }
        marker = Marker(map).apply {
            title = "R6Controller"
            setAnchor(Marker.ANCHOR_CENTER, Marker.ANCHOR_BOTTOM)
        }
        map.overlays.add(trail)
        map.overlays.add(marker)

        findViewById<Button>(R.id.btnBack).setOnClickListener { finish() }

        lifecycleScope.launch {
            repeatOnLifecycle(Lifecycle.State.STARTED) {
                launch { CarLink.gps.collect { showGps(it) } }
                launch {
                    CarLink.track.collect { pts ->
                        trail.setPoints(pts.map { GeoPoint(it.lat, it.lon) })
                        map.invalidate()
                    }
                }
            }
        }
    }

    private fun showGps(g: GpsData?) {
        if (g == null) {
            info.text = "Sin datos del carro"
            return
        }
        if (!g.fix) {
            info.text = "Buscando satélites… (${g.sats})"
            return
        }
        info.text = String.format(
            Locale.US, "Lat %.6f  Lon %.6f\n%d satélites · %.1f km/h",
            g.lat, g.lon, g.sats, g.speedKmh
        )
        val p = GeoPoint(g.lat, g.lon)
        marker.position = p
        if (!centeredOnce) {
            map.controller.animateTo(p)
            centeredOnce = true
        }
        map.invalidate()
    }

    override fun onResume() {
        super.onResume()
        map.onResume()
    }

    override fun onPause() {
        super.onPause()
        map.onPause()
    }

    private fun hideSystemBars() {
        WindowCompat.setDecorFitsSystemWindows(window, false)
        WindowInsetsControllerCompat(window, window.decorView).apply {
            hide(WindowInsetsCompat.Type.systemBars())
            systemBarsBehavior = WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
        }
    }
}
