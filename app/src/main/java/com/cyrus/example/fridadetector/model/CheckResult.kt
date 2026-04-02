package com.cyrus.example.fridadetector.model

data class CheckResult(
    val type: CheckType,
    val success: Boolean,
    val message: String
)