#include <jni.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <android/log.h>
#include <pthread.h>

#define TAG "FridaPortCheck"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

static JavaVM *g_vm = nullptr;
static jclass g_cls = nullptr;
static jmethodID g_mid = nullptr;


JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;

    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    jclass local = env->FindClass(
            "com/cyrus/example/fridadetector/core/checks/FridaPortCheck"
    );

    // ⚠️ 转成全局引用
    g_cls = (jclass) env->NewGlobalRef(local);
    env->DeleteLocalRef(local);

    g_mid = env->GetStaticMethodID(
            g_cls,
            "onNativeResult",
            "(ILjava/lang/String;)V"
    );

    return JNI_VERSION_1_6;
}

void notify_java(int port, const char *msg) {

    JNIEnv *env = nullptr;
    bool needDetach = false;

    if (g_vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        g_vm->AttachCurrentThread(&env, nullptr);
        needDetach = true;
    }

    jstring jmsg = env->NewStringUTF(msg);

    env->CallStaticVoidMethod(g_cls, g_mid, port, jmsg);

    env->DeleteLocalRef(jmsg);

    if (needDetach) {
        g_vm->DetachCurrentThread();
    }
}

static int set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// WebSocket 握手请求
static const char *ws_req =
        "GET /ws HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "User-Agent: Frida/16.7.19\r\n"
        "\r\n";

// 线程函数
void *scan_ports_thread(void *arg) {

    struct sockaddr_in sa{};
    sa.sin_family = AF_INET;
    inet_aton("127.0.0.1", &sa.sin_addr);

    char recv_buf[1024];

    for (int port = 1; port <= 65535; port++) {

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;

        set_nonblock(sock);

        sa.sin_port = htons(port);

        bool is_frida = false;

        do {
            connect(sock, (struct sockaddr *) &sa, sizeof(sa));

            fd_set wfds;
            FD_ZERO(&wfds);
            FD_SET(sock, &wfds);

            struct timeval tv{};
            tv.tv_sec = 0;
            tv.tv_usec = 100 * 1000;

            int sel = select(sock + 1, NULL, &wfds, NULL, &tv);
            if (!(sel > 0 && FD_ISSET(sock, &wfds))) break;

            int err = 0;
            socklen_t len = sizeof(err);
            getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len);

            if (err != 0) break;

            LOGD("[+] Port %d CONNECTED", port);

            // ===== WebSocket 握手 =====
            send(sock, ws_req, strlen(ws_req), 0);

            usleep(100 * 1000);

            memset(recv_buf, 0, sizeof(recv_buf));
            int r = recv(sock, recv_buf, sizeof(recv_buf) - 1, MSG_DONTWAIT);

            if (r <= 0) {
                LOGD("[+] Port %d NO RESPONSE", port);
                break;
            }

            LOGD("[+] Port %d RESPONSE (%d bytes):\n%s", port, r, recv_buf);

            // ===== 判断 WebSocket =====
            if (strstr(recv_buf, "HTTP/1.1 101") &&
                strstr(recv_buf, "Upgrade: websocket") &&
                strstr(recv_buf, "Connection: Upgrade") &&
                strstr(recv_buf, "Sec-WebSocket-Accept")) {

                LOGD("[!!!] WebSocket detected on port %d", port);

                is_frida = true;
                break;
            }

        } while (false);

        // ===== 释放资源 =====
        close(sock);

        // ===== 命中后退出 =====
        if (is_frida) {
            notify_java(port, "FRIDA WEBSOCKET DETECTED");
            return nullptr;
        }
    }

    LOGD("[*] Scan finished");
    notify_java(-1, "SCAN_FINISHED");

    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_cyrus_example_fridadetector_core_checks_FridaPortCheck_nativeStartScan(
        JNIEnv *env, jobject thiz) {

    pthread_t tid;

    if (pthread_create(&tid, nullptr, scan_ports_thread, nullptr) == 0) {

        pthread_detach(tid); // 线程结束自动回收资源

        LOGD("[*] Scan thread started");

    } else {
        LOGD("[!] Failed to create thread");
    }
}

