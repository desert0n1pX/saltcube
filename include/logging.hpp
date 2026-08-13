/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef LOGGING_H
#define LOGGING_H

#define TERM_HOME   "\033[H"

#define TERM_ERASE_TO_END           "\033[J"
#define TERM_ERASE_AFTER_CURSOR     "\033[0J"
#define TERM_ERASE_BEFORE_CURSOR    "\033[1J"
#define TERM_ERASE_SCREEN           "\033[2J"
#define TERM_ERASE_SAVED            "\033[3J"
#define TERM_ERASE_INLINE           "\033[K"
#define TERM_ERASE_LINE_REMAIN      "\033[0K"
#define TERM_ERASE_LINE_PREV        "\033[1K"
#define TERM_ERASE_LINE             "\033[2K"

#define TERM_FORMAT_RESET   "\033[0m"
#define TERM_FORMAT_BOLD    "\033[1m"
#define TERM_FORMAT_DIM     "\033[2m"
#define TERM_FORMAT_ITALIC  "\033[3m"
#define TERM_FORMAT_UNDER   "\033[4m"
#define TERM_FORMAT_BLINK   "\033[5m"
#define TERM_FORMAT_INVERT  "\033[7m"
#define TERM_FORMAT_HIDDEN  "\033[8m"
#define TERM_FORMAT_STRIKET "\033[9m"

#define COLOR_FG_RGB(r,g,b)    "\033[38;2;"#r";"#g";"#b"m"
#define COLOR_BG_RGB(r,g,b)    "\033[48;2;"#r";"#g";"#b"m"

#define COLOR_FG_BLACK      "\033[30m"
#define COLOR_FG_RED        "\033[31m"
#define COLOR_FG_GREEN      "\033[32m"
#define COLOR_FG_YELLOW     "\033[33m"
#define COLOR_FG_BLUE       "\033[34m"
#define COLOR_FG_PURPLE     "\033[35m"
#define COLOR_FG_LIGHTBLUE  "\033[36m"
#define COLOR_FG_WHITE      "\033[37m"
#define COLOR_FG_DEFAULT    "\033[39m"

#define COLOR_BG_BLACK      "\033[40m"
#define COLOR_BG_RED        "\033[41m"
#define COLOR_BG_GREEN      "\033[42m"
#define COLOR_BG_YELLOW     "\033[43m"
#define COLOR_BG_BLUE       "\033[44m"
#define COLOR_BG_PURPLE     "\033[45m"
#define COLOR_BG_LIGHTBLUE  "\033[46m"
#define COLOR_BG_WHITE      "\033[47m"
#define COLOR_BG_DEFAULT    "\033[49m"

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