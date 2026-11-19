package com.example.r6controller

import android.content.Context
import android.graphics.Canvas
import android.graphics.Color
import android.graphics.Paint
import android.util.AttributeSet
import android.view.MotionEvent
import android.view.View
import kotlin.math.hypot
import kotlin.math.min
import kotlin.math.roundToInt

/**
 * Joystick circular (gris con perilla negra, como en el boceto).
 * Entrega x,y entre -100 y 100 (y positivo = adelante). Al soltar vuelve al centro.
 */
class JoystickView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
) : View(context, attrs) {

    var onMove: ((x: Int, y: Int) -> Unit)? = null

    private val basePaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.GRAY; style = Paint.Style.FILL; alpha = 200
    }
    private val borderPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply {
        color = Color.BLACK; style = Paint.Style.STROKE; strokeWidth = 6f
    }
    private val knobPaint = Paint(Paint.ANTI_ALIAS_FLAG).apply { color = Color.BLACK }

    private var cx = 0f
    private var cy = 0f
    private var baseR = 0f
    private var knobR = 0f
    private var knobX = 0f
    private var knobY = 0f
    private var pointerId = -1

    override fun onSizeChanged(w: Int, h: Int, oldw: Int, oldh: Int) {
        cx = w / 2f
        cy = h / 2f
        baseR = min(w, h) / 2f - borderPaint.strokeWidth
        knobR = baseR * 0.3f
        knobX = cx
        knobY = cy
    }

    override fun onDraw(canvas: Canvas) {
        canvas.drawCircle(cx, cy, baseR, basePaint)
        canvas.drawCircle(cx, cy, baseR, borderPaint)
        canvas.drawCircle(knobX, knobY, knobR, knobPaint)
    }

    override fun onTouchEvent(e: MotionEvent): Boolean {
        when (e.actionMasked) {
            MotionEvent.ACTION_DOWN -> {
                pointerId = e.getPointerId(0)
                parent?.requestDisallowInterceptTouchEvent(true)
                update(e.x, e.y)
            }
            MotionEvent.ACTION_MOVE -> {
                val i = e.findPointerIndex(pointerId)
                if (i >= 0) update(e.getX(i), e.getY(i))
            }
            MotionEvent.ACTION_POINTER_UP -> {
                if (e.getPointerId(e.actionIndex) == pointerId) reset()
            }
            MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                performClick()
                reset()
            }
        }
        return true
    }

    override fun performClick(): Boolean = super.performClick()

    private fun update(x: Float, y: Float) {
        var dx = x - cx
        var dy = y - cy
        val max = baseR - knobR
        val d = hypot(dx, dy)
        if (d > max) {
            dx *= max / d
            dy *= max / d
        }
        knobX = cx + dx
        knobY = cy + dy
        invalidate()
        onMove?.invoke((dx / max * 100).roundToInt(), (-dy / max * 100).roundToInt())
    }

    fun reset() {
        pointerId = -1
        knobX = cx
        knobY = cy
        invalidate()
        onMove?.invoke(0, 0)
    }
}
