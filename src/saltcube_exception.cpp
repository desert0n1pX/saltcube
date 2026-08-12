/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "saltcube_exception.hpp"

#include <format>
#include <string>

saltcube_exception::saltcube_exception (
                                        const char *      what,
                                        const std::string where,
                                        int               line,
                                        const std::string why) noexcept :
                                        what_failed(what),
                                        where_happened(std::format("{0}: {1}", where, std::to_string(line))),
                                        why_happened(why) {}


const char * saltcube_exception::what() const noexcept {
    return what_failed;
}

const std::string saltcube_exception::where() const noexcept {
    return where_happened.c_str();
}

const std::string saltcube_exception::why() const noexcept {
    return why_happened.c_str();
}

const char * saltcube_exception::full_message() noexcept {
    full_message_str = std::format("{0} failed in {1} because {2}", what_failed, where_happened, why_happened);

    return full_message_str.c_str();
}