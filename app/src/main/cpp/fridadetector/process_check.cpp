#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <memory>
#include <android/log.h>

#define TAG "ProcessCheck"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

static bool containsKeyword(const std::string &name,
                            const std::vector<std::string> &keywords) {
    for (const auto &kw : keywords) {
        if (name.find(kw) != std::string::npos) {
            return true;
        }
    }
    return false;
}

/**
 * 通过 su 执行 ps 获取完整进程列表
 */
static std::vector<std::string> scanByRoot(const std::vector<std::string> &keywords) {
    std::vector<std::string> results;

    // 构造 grep 正则
    std::string pattern;
    for (size_t i = 0; i < keywords.size(); i++) {
        pattern += keywords[i];
        if (i != keywords.size() - 1) pattern += "|";
    }

    std::string cmd = "su -c \"ps -A -o PID,NAME,ARGS\"";

    LOGD("exec: %s", cmd.c_str());

    FILE *fp = popen(cmd.c_str(), "r");
    if (!fp) {
        LOGD("popen failed");
        return results;
    }

    char buffer[512];
    while (fgets(buffer, sizeof(buffer), fp)) {
        std::string line(buffer);

        // 打印所有进程
        LOGD("ps: %s", line.c_str());

        if (!containsKeyword(line, keywords)) continue;

        // 简单解析：PID 在第一列
        std::istringstream iss(line);
        std::string pid, name, args;
        iss >> pid >> name;

        // 剩余部分就是 args（可能为空）
        std::getline(iss, args);

        // 去掉前导空格
        if (!args.empty() && args[0] == ' ') {
            args.erase(0, args.find_first_not_of(' '));
        }

        if (!pid.empty() && !name.empty()) {
            LOGD(">>> HIT pid=%s, name=%s, args=%s", pid.c_str(), name.c_str(), args.c_str());
            results.emplace_back(pid + ":" + name + ":" + args);
        }
    }

    pclose(fp);
    return results;
}

/**
 * fallback：原 /proc 扫描（非 root）
 */
#include <dirent.h>
#include <unistd.h>
#include <fstream>

static std::vector<std::string> scanByProc(const std::vector<std::string> &keywords) {
    std::vector<std::string> results;

    DIR *proc_dir = opendir("/proc");
    if (!proc_dir) return results;

    std::string current_pid = std::to_string(getpid());
    struct dirent *entry;

    while ((entry = readdir(proc_dir)) != nullptr) {
        if (entry->d_type != DT_DIR) continue;

        std::string pid(entry->d_name);
        if (!std::all_of(pid.begin(), pid.end(), ::isdigit) || pid == current_pid)
            continue;

        std::ifstream comm_file("/proc/" + pid + "/comm");
        if (!comm_file.is_open()) continue;

        std::string process_name;
        std::getline(comm_file, process_name);
        comm_file.close();

        LOGD("proc: pid=%s, comm=%s", pid.c_str(), process_name.c_str());

        if (containsKeyword(process_name, keywords)) {
            LOGD(">>> HIT pid=%s (%s)", pid.c_str(), process_name.c_str());
            results.emplace_back(pid + ":" + process_name);
        }
    }

    closedir(proc_dir);
    return results;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_cyrus_example_fridadetector_core_checks_ProcessCheck_findProcesses(
        JNIEnv *env,
        jobject thiz,
        jobjectArray keywords_) {

    std::vector<std::string> keywords;
    jsize kw_len = env->GetArrayLength(keywords_);

    for (jsize i = 0; i < kw_len; i++) {
        jstring jstr = (jstring) env->GetObjectArrayElement(keywords_, i);
        const char *cstr = env->GetStringUTFChars(jstr, nullptr);
        keywords.emplace_back(cstr);
        env->ReleaseStringUTFChars(jstr, cstr);
        env->DeleteLocalRef(jstr);
    }

    std::vector<std::string> results;

    // ✅ 优先 root 扫描
    results = scanByRoot(keywords);

    // ❗ fallback
    if (results.empty()) {
        LOGD("fallback to /proc scan");
        results = scanByProc(keywords);
    }

    // 构造返回
    jclass stringClass = env->FindClass("java/lang/String");
    jobjectArray ret = env->NewObjectArray(results.size(), stringClass, nullptr);

    for (size_t i = 0; i < results.size(); i++) {
        env->SetObjectArrayElement(
                ret,
                i,
                env->NewStringUTF(results[i].c_str())
        );
    }

    return ret;
}

