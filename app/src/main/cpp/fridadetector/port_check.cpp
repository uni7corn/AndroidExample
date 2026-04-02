#include <jni.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>


extern "C"
JNIEXPORT jboolean JNICALL
Java_com_cyrus_example_fridadetector_core_checks_PortCheck_checkPort(
        JNIEnv *env,
        jobject thiz,
        jint port) {

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return JNI_FALSE;
    }

    struct sockaddr_in sa{};
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    inet_aton("127.0.0.1", &sa.sin_addr);

    int result = connect(sock, (struct sockaddr *) &sa, sizeof(sa));

    if (result == 0) {
        close(sock);
        return JNI_TRUE;
    }

    close(sock);
    return JNI_FALSE;
}
