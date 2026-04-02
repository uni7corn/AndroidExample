package com.cyrus.example.fridadetector.core.checks

import com.cyrus.example.fridadetector.model.CheckResult
import com.cyrus.example.fridadetector.model.CheckType

object ProcessCheck {

    init {
        System.loadLibrary("fridadetector")
    }

    /**
     * 返回命中的进程信息列表：
     * ["pid:processName", ...]
     */
    external fun findProcesses(keywords: Array<String>): Array<String>

    fun check(): CheckResult {
        return try {
            val keywords = arrayOf("frida", "gum", "gadget")

            val results = findProcesses(keywords)
            val detected = results.isNotEmpty()

            val message = if (detected) {
                "Suspicious processes:\n${results.joinToString("\n")}"
            } else {
                "OK"
            }

            CheckResult(
                CheckType.PROCESS,
                !detected,
                message
            )
        } catch (e: Exception) {
            CheckResult(CheckType.PROCESS, false, "ERROR: ${e.message}")
        }
    }

}