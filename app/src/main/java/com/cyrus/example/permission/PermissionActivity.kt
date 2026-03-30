package com.cyrus.example.permission

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Button
import androidx.compose.material3.LocalTextStyle
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.darkColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.input.TextFieldValue
import androidx.compose.ui.unit.dp

class PermissionActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContent {
            MaterialTheme(colorScheme = darkColorScheme()) {
                PermissionScreen(defaultPackageName = packageName)
            }
        }
    }
}

@Composable
fun PermissionScreen(defaultPackageName: String) {
    val context = LocalContext.current
    var pkg by remember { mutableStateOf(TextFieldValue(defaultPackageName)) }

    var showAppList by remember { mutableStateOf(false) }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(20.dp),
        verticalArrangement = Arrangement.Top,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {

        Text(
            "Permission Test",
            style = MaterialTheme.typography.titleLarge,
            color = MaterialTheme.colorScheme.onBackground
        )

        Spacer(modifier = Modifier.height(20.dp))

        Row(modifier = Modifier.fillMaxWidth()) {

            OutlinedTextField(
                value = pkg,
                onValueChange = { pkg = it },
                label = { Text("Package Name", color = MaterialTheme.colorScheme.onBackground) },
                modifier = Modifier.weight(1f),
                textStyle = LocalTextStyle.current.copy(color = MaterialTheme.colorScheme.onBackground)
            )

            Spacer(modifier = Modifier.width(10.dp))

            Button(
                onClick = { showAppList = true },
                modifier = Modifier.align(Alignment.CenterVertically)
            ) {
                Text("选择 App", color = MaterialTheme.colorScheme.onPrimary)
            }
        }

        Spacer(modifier = Modifier.height(20.dp))

        PermissionButton("打开 App 详情页") {
            PermissionUtils.openAppDetails(it, pkg.text)
        }

        PermissionButton("打开通知权限") {
            PermissionUtils.openNotificationSettings(it, pkg.text)
        }

        PermissionButton("悬浮窗权限（Overlay）") {
            PermissionUtils.openOverlayPermission(it, pkg.text)
        }

        PermissionButton("写系统设置权限") {
            PermissionUtils.openWriteSettings(it, pkg.text)
        }

        PermissionButton("忽略电池优化权限") {
            PermissionUtils.openBatteryIgnore(it, pkg.text)
        }
    }

    if (showAppList) {
        AppListDialog(
            onDismiss = { showAppList = false },
            onSelect = { appInfo ->
                pkg = TextFieldValue(appInfo.packageName)
                showAppList = false
            }
        )
    }
}


@Composable
fun PermissionButton(text: String, onClick: (android.content.Context) -> Unit) {
    val context = LocalContext.current

    Button(
        onClick = { onClick(context) },
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 6.dp)
            .height(48.dp)
    ) {
        Text(text, color = MaterialTheme.colorScheme.onPrimary)
    }
}
