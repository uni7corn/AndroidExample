package com.cyrus.example.fridadetector.core.checks

import android.util.Log
import com.cyrus.example.fridadetector.model.CheckResult
import com.cyrus.example.fridadetector.model.CheckType

object ThreadCheck {

    private const val TAG = "ThreadCheck"

    fun check(): CheckResult {
        val threadMap = Thread.getAllStackTraces()

        val detectedThreads = mutableListOf<String>()

        val logBuilder = StringBuilder()
        logBuilder.append("=== Thread List (${threadMap.size}) ===\n")

        threadMap.forEach { (thread, stackTrace) ->
            val info = buildString {
                append("id=${thread.id}, ")
                append("name=${thread.name}, ")
                append("state=${thread.state}, ")
                append("daemon=${thread.isDaemon}")
            }

            logBuilder.append(info).append("\n")

            // 打印前几行栈（可选）
            stackTrace.take(5).forEach {
                logBuilder.append("    at $it\n")
            }

            // 检测
            if (thread.name.contains("gum", true) ||
                thread.name.contains("frida", true)
            ) {
                detectedThreads.add(info)
            }
        }

        Log.d(TAG, logBuilder.toString())

        val detected = detectedThreads.isNotEmpty()

        return CheckResult(
            CheckType.THREAD,
            !detected,
            if (detected)
                "Detected:\n${detectedThreads.joinToString("\n")}"
            else
                "OK"
        )
    }
}