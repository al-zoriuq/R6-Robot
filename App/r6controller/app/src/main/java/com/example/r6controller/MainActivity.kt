package com.example.r6controller

import android.content.Intent
import android.os.Bundle
import android.view.View
import android.view.WindowManager
import android.widget.Button
import android.widget.ImageView
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.lifecycleScope
import androidx.lifecycle.repeatOnLifecycle
import kotlinx.coroutines.launch

/** Pantalla principal: video + joystick + botón "Ver ubicación en el mapa". */
class MainActivity : AppCompatActivity() {

    private lateinit var video: ImageView
    private lateinit var status: TextView
    private lateinit var videoStatus: TextView
    private lateinit var joystick: JoystickView
    private lateinit var stream: MjpegStream

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
        hideSystemBars()

        video = findViewById(R.id.videoView)
        status = findViewById(R.id.statusText)
        videoStatus = findViewById(R.id.videoStatus)
        joystick = findViewById(R.id.joystick)

        CarLink.start(applicationContext)

        joystick.onMove = { x, y -> CarLink.setJoystick(x, y) }

        findViewById<Button>(R.id.btnMap).setOnClickListener {
            startActivity(Intent(this, MapActivity::class.java))
        }

        stream = MjpegStream(
            url = CarLink.CAM_URL,
            onFrame = { bmp -> video.setImageBitmap(bmp) },
            onStatus = { msg ->
                videoStatus.text = msg
                videoStatus.visibility = if (msg == null) View.GONE else View.VISIBLE
            },
        )

        lifecycleScope.launch {
            repeatOnLifecycle(Lifecycle.State.STARTED) {
                CarLink.state.collect { s ->
                    status.text = when (s) {
                        LinkState.SEARCHING_WIFI -> "● Conéctate al WiFi \"R6Controller\""
                        LinkState.CONNECTING -> "● Conectando…"
                        LinkState.CONNECTED -> "● Conectado"
                    }
                    status.setTextColor(
                        if (s == LinkState.CONNECTED) 0xFF4CAF50.toInt() else 0xFFFF5252.toInt()
                    )
                }
            }
        }
    }

    override fun onStart() {
        super.onStart()
        stream.start(lifecycleScope)   // el video solo corre mientras se ve esta pantalla
    }

    override fun onPause() {
        super.onPause()
        joystick.reset()               // por seguridad: el carro se detiene
    }

    override fun onStop() {
        super.onStop()
        stream.stop()
    }

    private fun hideSystemBars() {
        WindowCompat.setDecorFitsSystemWindows(window, false)
        WindowInsetsControllerCompat(window, window.decorView).apply {
            hide(WindowInsetsCompat.Type.systemBars())
            systemBarsBehavior = WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
        }
    }
}
