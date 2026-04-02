package com.cyrus.example.fridadetector.core.checks

import com.cyrus.example.fridadetector.model.CheckResult
import com.cyrus.example.fridadetector.model.CheckType

object FridaPortCheck {

    init {
        System.loadLibrary("fridadetector")
    }

    external fun nativeStartScan()

    // 用于接收 native 回调
    var listener: ((CheckResult) -> Unit)? = null

    @JvmStatic
    fun onNativeResult(port: Int, msg: String) {
        listener?.invoke(
            CheckResult(
                CheckType.FRIDA_PORT,
                port == -1,
                "Port=$port $msg"
            )
        )
    }

    fun check(): CheckResult {

        nativeStartScan()

        return CheckResult(
            CheckType.FRIDA_PORT,
            true,
            "Scanning..."
        )
    }
}