package com.cyrus.example.fridadetector.core.checks

import android.util.Log
import com.cyrus.example.fridadetector.model.CheckResult
import com.cyrus.example.fridadetector.model.CheckType
import java.io.File

object MapsCheck {

    private const val TAG = "MapsCheck"

    fun check(): CheckResult {
        return try {
            val keywords = listOf(
                "frida",
                "gum-js",
                "gmain"
            )

            val hitLines = mutableListOf<String>()

            File("/proc/self/maps").forEachLine { line ->
                if (keywords.any { line.contains(it, ignoreCase = true) }) {
                    Log.i(TAG, ">>> HIT: $line")
                    hitLines.add(line)
                }
            }

            val detected = hitLines.isNotEmpty()

            val message = if (detected) {
                "Frida detected in maps:\n${hitLines.joinToString("\n")}"
            } else {
                "OK"
            }

            CheckResult(
                CheckType.MAPS,
                !detected,
                message
            )
        } catch (e: Exception) {
            CheckResult(CheckType.MAPS, true, "Error: ${e.message}")
        }
    }
}