package com.cyrus.example.fridadetector.core.checks

import com.cyrus.example.fridadetector.model.*

object PortCheck {

    init {
        System.loadLibrary("fridadetector")
    }

    external fun checkPort(port: Int): Boolean

    /**
     * 返回命中的端口列表
     */
    fun findOpenPorts(): List<Int> {
        val ports = intArrayOf(27042, 27043)
        return ports.filter { checkPort(it) }
    }

    fun check(): CheckResult {
        val hitPorts = findOpenPorts()
        val detected = hitPorts.isNotEmpty()

        val message = if (detected) {
            "Frida port detected: ${hitPorts.joinToString(",")}"
        } else {
            "OK"
        }

        return CheckResult(
            CheckType.PORT,
            !detected,
            message
        )
    }

}