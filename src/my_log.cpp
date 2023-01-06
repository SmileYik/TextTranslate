// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#include "my_log.h"

void log(const String& text, Ostream os, const StringList& args) {
    String str = text;
    int idx;
    int argsIdx = 0;
    while (!str.empty()) {
        idx = str.find("${}");
        if (idx == String::npos) {
            os << str;
        } else {
            os << str.substr(0, idx);
            if (argsIdx < args.size()) {
                os << args[argsIdx];
                ++argsIdx;
            }
            idx += 3;
            str = str.substr(idx);
        }
    }
}

void debug(const String& str, const StringList& args) {
#ifdef ENABLE_DEBUG_LOG
    String text = "[debug] " + str;
    log(text, std::cout, args);
#endif // ENABLE_DEBUG_LOG
}

void debug(const String& str) {
    debug(str, {});
}

void MyLog::error(const String& str, const StringList& args) {
#ifdef ENABLE_ERROR_LOG
    String text = "[error] " + str;
    log(text, std::cerr, args);
#endif // ENABLE_ERROR_LOG
}

void error(const String& str) {
#ifdef ENABLE_ERROR_LOG
    String text = "[error] " + str;
    log(text, std::cerr, {});
#endif // ENABLE_ERROR_LOG
}


