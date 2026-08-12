/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef LOGGING_H
#define LOGGING_H

#include "saltcube_exception.hpp"
#include <exception>
#include <string>

#define DEFAULT_LOG_LEVEL "notice"

namespace logging {
    /**
    * @brief Level of logging
    * 
    */
    enum LOGLEVEL {
        ERROR,
        CRITICAL,
        WARNING,
        NOTICE,
        INFO,
        DEBUG
    };
    
    /**
    * @brief Get the loglevel
    * 
    * @return std::string 
    */
    std::string get_loglevel();

    /**
    * @brief Log a message
    * 
    * @param level Level to log at
    * @param message message to send
    */
    void log(enum LOGLEVEL level, std::string message);

    /**
    * @brief Log a message with exclusive console control
    * 
    * @param level Level to log at
    * @param message message to send
    */
    void atomic_log(enum LOGLEVEL level, std::string message);

    /**
    * @brief Log a detailed message
    * 
    * @param level 
    * @param src_file 
    * @param function 
    * @param message 
    */
    void log(enum LOGLEVEL level, std::string message, saltcube_exception &except);

    /**
    * @brief Log a detailed message
    * 
    * @param level 
    * @param src_file 
    * @param function 
    * @param message 
    */
    void log(enum LOGLEVEL level, std::string message, std::exception &except);

    /**
    * @brief Set the Log Level
    * 
    * @param logLevel New Log Level
    */
    void set_loglevel(enum LOGLEVEL logLevel);
    void set_loglevel(std::string);

}

#endif