// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT
#include "translate_content.h"
#include "google_translator.h"
#include "itranslator.h"
#include "rio.h"
#include <functional>
#include <cstring>

#define HEAD_INPUT 1
#define TAIL_INPUT 2

typedef std::function<void(int, char**)> ParamFunction;

typedef struct {
    String name;
    StringList alias;
    u_char argc;
    String description;
    ParamFunction function;
    bool usead;
} Param;

typedef std::vector<Param> ParamList;

int main(int argc, char **argv) {
    ITranslator* translator = nullptr;
    TranslateContent translateContent;
    std::vector<String*> inputList;
    std::vector<int> inputTypeList;
    bool inputContent = false;

    ParamList params = {
        {"translator", {"t"}, 1, "选择需要使用的翻译器, 目前可选类型有google",
        [&translator] (int idx, char** args) {
            char* translateType = args[idx];
            if (strcmp(translateType, "g") == 0 ||
                strcmp(translateType, "google") == 0) {
                translator = new GoogleTranslator();
            }
        }},
        {"source-language", {"sl", "s"}, 1, "指定要翻译的文本源语言",
        [&translateContent] (int idx, char** args) {
            translateContent.srcLang = args[idx];
        }},
        {"source-language-input", {"sli"}, 0, "立即在标准输入中读取源语言",
        [&translateContent, &inputList, &inputTypeList] (int idx, char** args) {
            inputList.push_back(&translateContent.srcLang);
            inputTypeList.push_back(HEAD_INPUT);
        }},
        {"source-language-input-back", {"slib"}, 0, "在标准输入中最后位置读取源语言",
        [&translateContent, &inputList, &inputTypeList] (int idx, char** args) {
            inputList.push_back(&translateContent.srcLang);
            inputTypeList.push_back(TAIL_INPUT);
        }},
        {"target-language", {"tl"}, 1, "指定要翻译成哪个语言",
        [&translateContent] (int idx, char** args) {
            translateContent.aimLang = args[idx];
        }},
        {"target-language-input", {"tli"}, 0, "立即在标准输入中读取要翻译成哪个语言",
        [&translateContent, &inputList, &inputTypeList] (int idx, char** args) {
            inputList.push_back(&translateContent.aimLang);
            inputTypeList.push_back(HEAD_INPUT);
        }},
        {"target-language-input-back", {"tlib"}, 0, "在标准输入中最后位置读取目标语言",
        [&translateContent, &inputList, &inputTypeList] (int idx, char** args) {
            inputList.push_back(&translateContent.aimLang);
            inputTypeList.push_back(TAIL_INPUT);
        }},
        {"input-content", {"i", "ic"}, 0, "读取标准输入流中的文本进行翻译",
        [&inputContent] (int idx, char** args) {
            inputContent = true;
        }}
    };

    Param helpParam =
    {"help", {"h"}, 0, "获取帮助信息.",
    [&params] (int idx, char** args) {
        String help = "Usage ";
        help += args[0];
        help += " \n";
        for (const auto& param : params) {
            help += "\t--" + param.name;
            String alias = "";
            for (const auto& a : param.alias) {
                alias += ", -" + a;
            }
            if (!alias.empty()) {
                help += " [ " + alias.substr(2) + " ]";
            }
            help += ": " + param.description + "\n";
        }
        printf("%s", help.c_str());
        exit(1);
    }};
    params.push_back(helpParam);

    if (argc == 1) {
        helpParam.function(argc, argv);
        return 1;
    }

    int i;
    for (i = 1; i < argc; ++i) {
        char* arg = argv[i];
        if (arg[0] != '-') {
            break;
        }
        Param targetParam;
        bool found = false;
        // find param.
        for (const auto& p : params) {
            if (("--" + p.name) == arg) {
                targetParam = p;
                found = true;
                break;
            }
            for (const auto& name : p.alias) {
                if (("-" + name) == arg) {
                    targetParam = p;
                    found = true;
                    break;
                }
            }
            if (found) {
                break;
            }
        }
        if (found) {
            if (targetParam.usead) {                // 如果已经使用过.
                if (targetParam.argc > 0) {
                    if (targetParam.argc + i >= argc) {
                        fprintf(stderr,
                                "Incorrect number of arguments supplied for '%s'.\n",
                                arg);
                        return -1;
                    }
                    i += targetParam.argc;
                }
                fprintf(stderr,
                        "Parameter '%s' has already been used.\n",
                        arg);
            } else if (targetParam.argc > 0) {      // 判断是否需要计算参数.
                // 提供错误的参数个数.
                if (targetParam.argc + i >= argc) {
                    fprintf(stderr,
                            "Incorrect number of arguments supplied for '%s'.\n",
                            arg);
                    return -1;
                }
                targetParam.function(i + 1, argv);
                i += targetParam.argc;
            } else {
                targetParam.function(i, argv);
            }
            targetParam.usead = true;
        } else {
            fprintf(stderr, "'%s' is an invalid parameter.\n", arg);
        }
    }

    if (translator == nullptr) {
        fprintf(stderr, "translator invalid\n");
        return -1;
    }

    String content = "";
    if (inputContent) {     // 从标准输入流中读取内容.
        char buffer[RIO_BUFFER_SIZE];
        int n;
        if ((n = Rio::rio_readn(0, (void *) buffer, RIO_BUFFER_SIZE)) > 0) {
            content.append(buffer, 0, n);
        }

        // 依照顺序读取目标语言.
        int typeIdx = 0;
        for (auto& str : inputList) {
            char tmp[50];
            if (inputTypeList[typeIdx] == HEAD_INPUT) {
                sscanf(content.c_str(), "%s", tmp);
                *str = tmp;
                content = content.substr(str->length() + 1);
            } else if (inputTypeList[typeIdx] == TAIL_INPUT) {
                auto c = content[content.length() - 1];
                while (c == ' ' || c == '\n') {
                    content = content.substr(0, content.length() - 1);
                    c = content[content.length() - 1];
                }
                i = content.find_last_of(" ");
                int ii = content.find_last_of("\n");
                if (ii > i) {
                    i = ii;
                }

                *str = content.substr(i + 1);
                content = content.substr(0, i);
            }
            ++typeIdx;
        }
    } else if (i < argc) {  // 从参数中读取内容.
        for (; i < argc; ++i) {
            content += " ";
            content += argv[i];
        }
        if (!content.empty()) {
            content = content.replace(0, 1, "");
        }
        // 依照顺序读取目标语言.
        for (auto& str : inputList) {
            char tmp[50];
            scanf("%s", tmp);
            *str = tmp;
        }
    }

    translateContent.text = content;

    if (ENABLE_DEBUG_LOG) {
        printf("---INPUT---\n"
               "source: %s\n"
               "target: %s\n"
               "%s\n"
               "------------\n", translateContent.srcLang.c_str(),
                                 translateContent.aimLang.c_str(),
                                 translateContent.text.c_str());
    }

    if (content.empty()) {
        printf("");
        return 0;
    }

    // 翻译.
    String result;
    bool success = translator->translate(translateContent, &result);
    free(translator);

    if (!success) {
        fprintf(stderr, "Translate failed!\n");
        return -1;
    }
    printf("%s", result.c_str());
    return 0;
}
