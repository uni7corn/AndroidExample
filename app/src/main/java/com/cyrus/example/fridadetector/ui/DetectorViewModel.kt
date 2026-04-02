package com.cyrus.example.fridadetector.ui

import androidx.lifecycle.ViewModel
import com.cyrus.example.fridadetector.core.FridaDetector
import com.cyrus.example.fridadetector.core.checks.FridaPortCheck
import com.cyrus.example.fridadetector.model.CheckItem
import com.cyrus.example.fridadetector.model.CheckResult
import com.cyrus.example.fridadetector.model.CheckType
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.update

class DetectorViewModel : ViewModel() {

    data class UiState(
        val items: List<CheckItem> = CheckType.entries.map { CheckItem(it) },
        val results: List<CheckResult> = emptyList()
    )

    private val _state = MutableStateFlow(UiState())
    val state = _state

    init {
        // 👉 注册 native 回调
        FridaPortCheck.listener = { result ->
            _state.update {
                it.copy(results = it.results + result)
            }
        }
    }

    fun toggle(type: CheckType) {
        _state.update {
            it.copy(items = it.items.map {
                if (it.type == type) it.copy(enabled = !it.enabled) else it
            })
        }
    }

    fun runChecks() {
        val selected = _state.value.items.filter { it.enabled }.map { it.type }

        val results = FridaDetector.runChecks(selected)

        _state.update {
            it.copy(results = results)
        }
    }
}