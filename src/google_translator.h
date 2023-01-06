// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#ifndef GOOGLE_TRANSLATOR_H
#define GOOGLE_TRANSLATOR_H

#include "itranslator.h"
#include "translate_content.h"

#define GOOGLE_TRANSLATOR_URL \
        "http://translate.googleapis.com/translate_a/single"

/**
 * google 翻译器.
 */
class GoogleTranslator : public ITranslator {
public:
    GoogleTranslator();
    ~GoogleTranslator();
    virtual bool translate(const TranslateContent & content,
                           String * translated) override;
};

#endif // GOOGLE_TRANSLATOR_H
