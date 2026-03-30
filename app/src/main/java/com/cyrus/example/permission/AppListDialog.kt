package com.cyrus.example.permission

import android.content.pm.ApplicationInfo
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.heightIn
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.core.graphics.drawable.toBitmap


@Composable
fun AppListDialog(
    onDismiss: () -> Unit,
    onSelect: (ApplicationInfo) -> Unit
) {
    val context = LocalContext.current
    val pm = context.packageManager

    val apps = remember {
        pm.getInstalledApplications(0)
            .sortedBy { pm.getApplicationLabel(it).toString() }
    }

    AlertDialog(
        onDismissRequest = onDismiss,
        title = {
            Text(
                "选择一个应用",
                color = MaterialTheme.colorScheme.onBackground
            )
        },
        text = {
            LazyColumn(
                modifier = Modifier.height(450.dp)
            ) {
                items(apps) { app ->
                    val label = pm.getApplicationLabel(app).toString()
                    val icon = pm.getApplicationIcon(app)

                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .heightIn(min = 48.dp, max = 60.dp)
                            .clickable { onSelect(app) }
                            .padding(horizontal = 8.dp, vertical = 6.dp),
                        verticalAlignment = Alignment.CenterVertically
                    ) {

                        // 应用图标
                        androidx.compose.foundation.Image(
                            bitmap = icon.toBitmap().asImageBitmap(),
                            contentDescription = null,
                            modifier = Modifier.size(32.dp)
                        )

                        Spacer(modifier = Modifier.width(10.dp))

                        Column(modifier = Modifier.weight(1f)) {
                            Text(
                                text = label,
                                color = MaterialTheme.colorScheme.onBackground
                            )
                            Text(
                                text = app.packageName,
                                color = MaterialTheme.colorScheme.onBackground.copy(alpha = 0.7f),
                                fontSize = MaterialTheme.typography.bodySmall.fontSize
                            )
                        }
                    }
                }
            }
        },
        confirmButton = {
            TextButton(onClick = onDismiss) {
                Text("关闭", color = MaterialTheme.colorScheme.onPrimaryContainer)
            }
        }
    )
}


//fun Drawable.toBitmap(): Bitmap {
//    val bmp = Bitmap.createBitmap(intrinsicWidth, intrinsicHeight, Bitmap.Config.ARGB_8888)
//    val canvas = Canvas(bmp)
//    setBounds(0, 0, canvas.width, canvas.height)
//    draw(canvas)
//    return bmp
//}
