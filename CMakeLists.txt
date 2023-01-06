cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(text_translate)

option(ENABLE_DEBUG_LOG "允许打印调试信息" OFF)
option(ENABLE_ERROR_LOG "允许打印错误信息" ON)

include_directories(
    thirdpart/cJSON/
)

set(
    text_translate_cpp
    src/translate_content.cpp
    src/google_translator.cpp
    src/rio.cpp
    src/http_action.cpp
    src/itranslator.cpp
    thirdpart/cJSON/cJSON.c
)

add_executable(text_translate src/main.cpp ${text_translate_cpp})

# 打印调试文本.
if (ENABLE_DEBUG_LOG)
    message("允许程序打印调试文本信息.")
    add_definitions("-DENABLE_DEBUG_LOG")
endif()

# 打印错误信息.
if (DENABLE_ERROR_LOG)
    message("允许程序打印错误信息.")
    add_definitions("-DENABLE_ERROR_LOG")
endif()
