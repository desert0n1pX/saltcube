/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "logging.hpp"
#include "options.hpp"
#include "saltcube_exception.hpp"

#include <exception>
#include <format>
#include <iostream>
#include <mutex>
#include <string>

#define UNKNOWN_LOGLEVEL_ERROR "Unknown loglevel"


std::mutex exclusive;

enum logging::LOGLEVEL log_level = logging::ERROR;

void logging::atomic_log(enum LOGLEVEL level, std::string message){
    std::scoped_lock<std::mutex> lock(exclusive);
    log(level, message);
}

std::string logging::get_loglevel() {
    switch (log_level) {
        case ERROR:
            return "ERROR";
            break;
        case CRITICAL:
            return "CRITICAL";
            break;
        case WARNING:
            return "WARNING";
            break;
        case NOTICE:
            return "NOTICE";
            break;
        case INFO:
            return "INFO";
            break;
        case DEBUG:
            return "DEBUG";
            break;
    }
    return "";
}

void logging::log(enum LOGLEVEL level, std::string message) {
    switch (level) {

    case ERROR:
        if (log_level >= ERROR)
        std::cout << "[ERROR]: " << message << std::endl;
        break;
    case CRITICAL:
        if (log_level >= CRITICAL)
        std::cout << "[CRITICAL]: " << message << std::endl;
        break;
    case WARNING:
        if (log_level >= WARNING)
        std::cout << "[WARNING]: " << message << std::endl;
        break;
    case NOTICE:
        if (log_level >= NOTICE)
        std::cout << "[NOTICE]: " << message << std::endl;
        break;
    case INFO:
        if (log_level >= INFO)
        std::cout << "[INFO]: " << message << std::endl;
        break;
    case DEBUG:
        if (log_level >= DEBUG)
        std::cout << "[DEBUG]: " << message << std::endl;
        break;
    }
}

void logging::log(enum LOGLEVEL level, std::string message, saltcube_exception &except){
    log(DEBUG, except.full_message());
    log(level, message);
}

void logging::log(enum LOGLEVEL level, std::string message, std::exception &except){
    log(DEBUG, std::format("Hit an exception! std::exception.what(): {0}", except.what()));
    log(level, message);
}

void logging::set_loglevel(enum LOGLEVEL desired_log_level){
    log_level = desired_log_level;
    log(NOTICE, "Set loglevel to " + get_loglevel());
}

void logging::set_loglevel(std::string desired_log_level){
    if (desired_log_level == "error") {
        log_level = INFO;

    } else if (desired_log_level == "critical") {
        log_level = CRITICAL;  

    } else if (desired_log_level == "warning") {
        log_level = WARNING;
    
    } else if (desired_log_level == "notice") {
        log_level = NOTICE;
    
    } else if (desired_log_level == "info") {
        log_level = INFO;
    
    } else if (desired_log_level == "debug") {
        log_level = DEBUG;
    
    } else {
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, UNKNOWN_LOGLEVEL_ERROR);
    }
}

