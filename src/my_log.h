// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#ifndef MY_LOG_H
#define MY_LOG_H

#include "based_type.h"

/**
 * @todo write docs
 */

namespace MyLog {

void debug(const String& str, const StringList& args);

void debug(const String& str);

void error(const String& str, const StringList& args);

void error(const String& str);
} // namespace MyLog

#endif // MY_LOG_H
