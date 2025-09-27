// SignatureUtils.kt
package com.cyrus.example.signature

import android.content.Context
import android.content.pm.PackageManager
import android.os.Build
import android.util.Base64
import android.util.Log
import java.security.MessageDigest

object SignatureUtils {
    private const val TAG = "SignatureUtils"

    /** 返回当前 app 的签名指纹列表（Base64(SHA256(certBytes))） */
    fun getSigningSha256Base64(context: Context): List<String> {
        val res = mutableListOf<String>()
        try {
            val pm = context.packageManager
            val pkg = context.packageName
            val packageInfo = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                pm.getPackageInfo(pkg, PackageManager.GET_SIGNING_CERTIFICATES)
            } else {
                @Suppress("DEPRECATION")
                pm.getPackageInfo(pkg, PackageManager.GET_SIGNATURES)
            }

            val signatures = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                packageInfo.signingInfo.apkContentsSigners
            } else {
                @Suppress("DEPRECATION")
                packageInfo.signatures
            }

            for (sig in signatures) {
                val certBytes = sig.toByteArray()
                val md = MessageDigest.getInstance("SHA-256")
                val digest = md.digest(certBytes)
                val base64 = Base64.encodeToString(digest, Base64.NO_WRAP)
                Log.d(TAG, "Runtime sign sha256 base64: $base64")
                res.add(base64)
            }
        } catch (e: Exception) {
            Log.e(TAG, "getSigningSha256Base64 error", e)
        }
        return res
    }

    /** 检查任一签名是否与期望值相等 */
    fun isSignedWith(context: Context, expectedBase64: String): Boolean {
        val list = getSigningSha256Base64(context)
        for (s in list) if (s == expectedBase64) return true
        return false
    }
}