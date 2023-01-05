cmake_minimum_required(VERSION 3.0)

project(text_translate)

add_executable(text_translate main.cpp)

install(TARGETS text_translate RUNTIME DESTINATION bin)
