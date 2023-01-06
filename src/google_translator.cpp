// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#include "google_translator.h"
#include "http_action.h"
#include <cJSON.h>

GoogleTranslator::GoogleTranslator() : ITranslator() {
}

GoogleTranslator::~GoogleTranslator() {
}



bool GoogleTranslator::translate(const TranslateContent& content, String* translated) {
    // client=gtx&dt=t&sl=en&tl=zh-CN&q=Let'stakeasentence
    String resp;
    int code = HttpAction::get(
        GOOGLE_TRANSLATOR_URL,
        {
            {"client", "gtx"},
            {"dt", "t"},
            {"sl", content.srcLang.empty() ? "auto" : content.srcLang},
            {"tl", content.aimLang},
            {"q", content.text}
        },
        &resp
    );

    if (code == 200) {
        cJSON* json = cJSON_Parse(resp.c_str());
        if (ENABLE_DEBUG_LOG) {
            printf("%s\n%s\n", resp.c_str(), cJSON_Print(json));
        }
        cJSON* translateArray = cJSON_GetArrayItem(json, 0);
        int size = cJSON_GetArraySize(translateArray);
        *translated = "";
        for (int i = 0; i < size; ++i) {
            translated->append(cJSON_GetArrayItem(
                cJSON_GetArrayItem(translateArray, i), 0)->valuestring);
        }
        cJSON_Delete(json);
    }
    return code == 200;
}
