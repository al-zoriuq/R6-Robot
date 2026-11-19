package com.example.r6controller

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.isActive
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import okhttp3.Call
import okhttp3.Request
import java.io.BufferedInputStream
import java.io.InputStream

/**
 * Lee el video MJPEG de la ESP32-CAM y entrega cada cuadro como Bitmap.
 * Se reconecta solo si el video se corta.
 */
class MjpegStream(
    private val url: String,
    private val onFrame: (Bitmap) -> Unit,
    private val onStatus: (String?) -> Unit,
) {
    private var job: Job? = null
    @Volatile private var call: Call? = null

    fun start(scope: CoroutineScope) {
        if (job?.isActive == true) return
        job = scope.launch(Dispatchers.IO) {
            while (isActive) {
                val client = CarLink.streamClient()
                if (client == null) {
                    withContext(Dispatchers.Main) { onStatus("Esperando WiFi del carro…") }
                    delay(1000)
                    continue
                }
                try {
                    val c = client.newCall(Request.Builder().url(url).build())
                    call = c
                    c.execute().use { resp ->
                        val body = resp.body ?: error("Sin datos")
                        val input = BufferedInputStream(body.byteStream(), 64 * 1024)
                        withContext(Dispatchers.Main) { onStatus(null) }
                        while (isActive) {
                            val jpeg = readFrame(input) ?: break
                            val bmp = BitmapFactory.decodeByteArray(jpeg, 0, jpeg.size) ?: continue
                            withContext(Dispatchers.Main) { onFrame(bmp) }
                        }
                    }
                } catch (e: Exception) {
                    if (!isActive) break
                    withContext(Dispatchers.Main) { onStatus("Sin video, reintentando…") }
                    delay(1000)
                }
            }
        }
    }

    fun stop() {
        call?.cancel()
        job?.cancel()
        job = null
    }

    private fun readFrame(input: InputStream): ByteArray? {
        var length = -1
        while (true) {
            val line = readLine(input) ?: return null
            if (line.isEmpty()) {
                if (length > 0) break else continue
            }
            val idx = line.indexOf(':')
            if (idx > 0 && line.substring(0, idx).trim().equals("Content-Length", ignoreCase = true)) {
                length = line.substring(idx + 1).trim().toIntOrNull() ?: -1
            }
        }
        val buf = ByteArray(length)
        var off = 0
        while (off < length) {
            val r = input.read(buf, off, length - off)
            if (r == -1) return null
            off += r
        }
        return buf
    }

    private fun readLine(input: InputStream): String? {
        val sb = StringBuilder()
        while (true) {
            val c = input.read()
            if (c == -1) return null
            if (c == '\n'.code) return sb.toString()
            if (c != '\r'.code && sb.length < 256) sb.append(c.toChar())
        }
    }
}
