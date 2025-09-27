package com.cyrus.example.signature

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.unit.dp
import androidx.compose.ui.platform.LocalContext
import com.cyrus.example.BuildConfig


class SignatureActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            SignatureScreen()
        }
    }
}

@Composable
fun SignatureScreen() {
    val context = LocalContext.current
    var output by remember { mutableStateOf("日志输出区\n") }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.spacedBy(12.dp)
    ) {
        Button(
            onClick = {
                val sigs = SignatureUtils.getSigningSha256Base64(context)
                output = "当前签名指纹:\n" + sigs.joinToString("\n")
            },
            modifier = Modifier.fillMaxWidth()
        ) {
            Text("打印当前 App 签名指纹")
        }

        Button(
            onClick = {
                val expected = BuildConfig.EXPECTED_SIGNATURE
                val ok = SignatureUtils.isSignedWith(context, expected)
                output = "签名校验结果: $ok\n\n期望签名: $expected"
            },
            modifier = Modifier.fillMaxWidth()
        ) {
            Text("执行签名校验")
        }

        Spacer(modifier = Modifier.height(16.dp))

        Text(
            text = output,
            color = Color.White,
            modifier = Modifier
                .fillMaxWidth()
                .weight(1f)
        )
    }
}
