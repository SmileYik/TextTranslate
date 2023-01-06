// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#ifndef TRANSLATE_CONTENT_H
#define TRANSLATE_CONTENT_H

#include "based_type.h"

/**
 * 翻译内容.
 */
class TranslateContent
{
public:
    String srcLang; // 文本的源语言
    String aimLang; // 要翻译成的语言
    String text;    // 要翻译的文本

    TranslateContent();
    TranslateContent(String sl, String al, String t);
};

#endif // TRANSLATE_CONTENT_H
