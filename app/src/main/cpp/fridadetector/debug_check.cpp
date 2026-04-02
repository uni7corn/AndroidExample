#include <jni.h>
#include <sys/ptrace.h>
#include <errno.h>
#include <unistd.h>
#include <fstream>
#include <string>
#include <android/log.h>

#define TAG "DebugCheck"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_cyrus_example_fridadetector_core_checks_DebugCheck_isBeingTraced__(
        JNIEnv *env,
        jobject thiz) {

    errno = 0;

    // 尝试声明自己被 trace
    long result = ptrace(PTRACE_TRACEME, 0, nullptr, nullptr);

    if (result == -1) {
        LOGD("ptrace failed, errno=%d", errno);

        // EPERM：已经被其他调试器附加（典型 Frida / gdb）
        if (errno == EPERM) {
            LOGD(">>> DETECTED: already being traced");
            return JNI_TRUE;
        }
    } else {
        // 成功说明当前未被 trace，需要 detach 避免影响后续
        ptrace(PTRACE_DETACH, 0, nullptr, nullptr);
    }

    return JNI_FALSE;
}



/**
 * TracerPid 检测
 */
extern "C"
JNIEXPORT jint JNICALL
Java_com_cyrus_example_fridadetector_core_checks_DebugCheck_getTracerPid__(
        JNIEnv *env,
        jobject thiz) {

    std::ifstream status("/proc/self/status");
    if (!status.is_open()) {
        LOGD("open /proc/self/status failed");
        return -1;
    }

    std::string line;
    while (std::getline(status, line)) {
        if (line.find("TracerPid:") == 0) {

            int tracerPid = atoi(line.substr(10).c_str());

            LOGD("TracerPid=%d", tracerPid);

            if (tracerPid != 0) {
                LOGD(">>> tracer detected (pid=%d)", tracerPid);
            }

            return tracerPid;
        }
    }

    return 0;
}