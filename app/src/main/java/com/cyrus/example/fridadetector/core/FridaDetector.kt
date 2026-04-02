package com.cyrus.example.fridadetector.core

import com.cyrus.example.fridadetector.core.checks.MapsCheck
import com.cyrus.example.fridadetector.core.checks.PortCheck
import com.cyrus.example.fridadetector.core.checks.ProcessCheck
import com.cyrus.example.fridadetector.core.checks.DebugCheck
import com.cyrus.example.fridadetector.core.checks.FridaPortCheck
import com.cyrus.example.fridadetector.core.checks.ThreadCheck
import com.cyrus.example.fridadetector.model.*

object FridaDetector {

    fun runChecks(selected: List<CheckType>): List<CheckResult> {
        val results = mutableListOf<CheckResult>()

        selected.forEach {
            when (it) {
                CheckType.PROCESS -> results += ProcessCheck.check()
                CheckType.PORT -> results += PortCheck.check()
                CheckType.MAPS -> results += MapsCheck.check()
                CheckType.THREAD -> results += ThreadCheck.check()
                CheckType.DEBUG -> results += DebugCheck.check()
                CheckType.FRIDA_PORT -> results += FridaPortCheck.check()
            }
        }

        return results
    }
}