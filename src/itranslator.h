// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#ifndef ITRANSLATOR_H
#define ITRANSLATOR_H

#include "translate_content.h"

/**
 * 翻译器.
 * 其有一个子类:
 * @see google_translator.h
 */
class ITranslator {
public:
    /**
     * 翻译文本.
     * @param content 要翻译的内容.
     * @param translated 返回的翻译内容.
     * @return 如果翻译成功返回true, 反之则返回false.
     */
    virtual bool translate(const TranslateContent& content, String* translated);
};

#endif // ITRANSLATOR_H
