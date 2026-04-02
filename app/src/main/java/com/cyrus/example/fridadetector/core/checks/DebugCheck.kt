package com.cyrus.example.fridadetector.core.checks

import com.cyrus.example.fridadetector.model.CheckResult
import com.cyrus.example.fridadetector.model.CheckType

object DebugCheck {

    init {
        System.loadLibrary("fridadetector")
    }

    /**
     * return true -> 被调试
     */
    external fun isBeingTraced(): Boolean

    /**
     * return tracerPid（0 表示未被调试）
     */
    external fun getTracerPid(): Int

    fun check(): CheckResult {
        return try {
            val tracerPid = getTracerPid()
            val ptraceDetected = isBeingTraced()

            val detected = tracerPid != 0 || ptraceDetected

            val message = buildString {
                if (tracerPid != 0) {
                    append("TracerPid detected: $tracerPid\n")
                }
                if (ptraceDetected) {
                    append("ptrace detected\n")
                }
                if (!detected) {
                    append("OK")
                }
            }

            CheckResult(
                CheckType.DEBUG,
                !detected,
                message.trim()
            )
        } catch (e: Exception) {
            CheckResult(CheckType.DEBUG, true, "Error: ${e.message}")
        }
    }
}