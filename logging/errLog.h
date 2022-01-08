#ifndef VULKANTUT2_ERRLOG_H
#define VULKANTUT2_ERRLOG_H

#include <fmt/format.h>
#include <experimental/source_location>

#define LOG_ARGS(str, ...)   auto loc = std::experimental::source_location::current();\
                             auto file = std::string(loc.file_name());\
                             file = std::string(file, file.find_last_of('/'));\
                             fmt::print("| [{}] {}:{}r,{}c | [error] | ", file, loc.function_name(), loc.line(), loc.column());\
                             fmt::print(str "\n", __VA_ARGS__)

#define LOG(str)        auto loc = std::experimental::source_location::current();\
                        auto file = std::string(loc.file_name());\
                        file = std::string(file, file.find_last_of('/'));\
                        fmt::print("| [{}] {}:{}r,{}c | [error] | ", file, loc.function_name(), loc.line(), loc.column());\
                        fmt::print(str "\n")

#endif