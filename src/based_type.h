// SPDX-FileCopyrightText: 2023 SmileYik miskyle@outlook.com
// SPDX-License-Identifier: MIT

#ifndef BASED_TYPE_H
#define BASED_TYPE_H

#ifdef ENABLE_ERROR_LOG
#define ENABLE_ERROR_LOG 1
#endif
#ifndef ENABLE_ERROR_LOG
#define ENABLE_ERROR_LOG 0
#endif

#ifdef ENABLE_DEBUG_LOG
#define ENABLE_DEBUG_LOG 1
#endif
#ifndef ENABLE_DEBUG_LOG
#define ENABLE_DEBUG_LOG 0
#endif

#include <iostream>
#include <map>
#include <vector>

typedef std::ostream Ostream;
typedef std::string String;
typedef std::vector<String> StringList;

typedef std::map<String, String> StringMap;
typedef std::multimap<String, String> StringMultiMap;

#endif // BASED_TYPE_H
