package com.cyrus.example.fridadetector.ui

import androidx.compose.foundation.background
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Button
import androidx.compose.material3.Checkbox
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp


@Composable
fun DetectorScreen(viewModel: DetectorViewModel = viewModel()) {

    val state by viewModel.state.collectAsState()

    Column(modifier = Modifier.fillMaxSize().background(Color.White) .padding(16.dp)) {

        Text("Frida Detector", style = MaterialTheme.typography.titleLarge)

        Spacer(Modifier.height(16.dp))

        LazyColumn(modifier = Modifier.weight(1f)) {
            items(state.items) { item ->
                Row(
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Checkbox(
                        checked = item.enabled,
                        onCheckedChange = {
                            viewModel.toggle(item.type)
                        }
                    )
                    Text(item.type.title)
                }
            }
        }

        Button(
            onClick = { viewModel.runChecks() },
            modifier = Modifier.fillMaxWidth()
        ) {
            Text("Start Detection")
        }

        Spacer(Modifier.height(16.dp))

        Text("Result:")

        LazyColumn {
            items(state.results) {
                Text(
                    text = "[${it.type}] ${it.message}",
                    color = if (it.success) Color.Green else Color.Red
                )
            }
        }
    }

}