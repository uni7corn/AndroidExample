package com.cyrus.example.fridadetector

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import com.cyrus.example.fridadetector.ui.DetectorScreen

class FridaDetectorActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContent {
            DetectorScreen()
        }
    }
}