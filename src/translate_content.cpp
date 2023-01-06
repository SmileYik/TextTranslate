// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#include "itranslator.h"

TranslateContent::TranslateContent() = default;

TranslateContent::TranslateContent(String sl, String al, String t) :
     srcLang(sl), aimLang(al), text(t) {
}

