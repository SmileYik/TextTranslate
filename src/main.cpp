// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT
#include "translate_content.h"
#include "google_translator.h"
#include "itranslator.h"
#include "rio.h"

int main(int argc, char **argv) {
    // 如果小于4个参数则返回提示文本.
    if (argc < 4) {
        fprintf(stderr,
                "Usage: %s <translator> <source language> <traget language> <text>\n\n"
                "\t\ttranslator: google\n",
                argv[0]
        );
        return -1;
    }

    // 选择翻译器.
    ITranslator* translator = nullptr;
    String translatorType = argv[1];
    if (translatorType == "google" || translatorType == "g") {
        translator = new GoogleTranslator();
    }
    if (translator == nullptr) {
        fprintf(stderr, "'%s' is not a valid translator type!\n", argv[1]);
        return -1;
    }


    String text = "";
    if (argc > 4) {  // 如果参数大于4个参数那么把多的字符以空格拼接起来组成翻译文本.
        for (int i = 4; i < argc; ++i) {
            text += " ";
            text += argv[i];
        }
        if (!text.empty()) {
            text = text.replace(0, 1, "");
        }
    } else {  // 如果参数仅有4个则从标准输入中读取要翻译的文本.
        char buffer[RIO_BUFFER_SIZE];
        int n;
        if ((n = Rio::rio_readn(0, (void *) buffer, RIO_BUFFER_SIZE)) > 0) {
            text.append(buffer, 0, n);
        }
    }

    if (ENABLE_DEBUG_LOG) {
        printf("---INPUT---\n%s\n------------\n", text.c_str());
    }

    // 翻译.
    String result;
    bool success = translator->translate(TranslateContent(
                       argv[2], argv[3], text
                   ), &result);
    free(translator);

    if (!success) {
        fprintf(stderr, "Translate failed!\n");
        return -1;
    }
    printf("%s", result.c_str());
    return 0;
}
