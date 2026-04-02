package com.cyrus.example.fridadetector.model

enum class CheckType(val title: String) {
    PROCESS("Process Detection"),
    PORT("Port Scan"),
    MAPS("Maps Scan"),
    THREAD("Thread Scan"),
    DEBUG("Debug Detection"),
    FRIDA_PORT("Frida Server Port Scan"),
}

data class CheckItem(
    val type: CheckType,
    var enabled: Boolean = true
)