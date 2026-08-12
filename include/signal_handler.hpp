/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#define SIGCHECK check_signal(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__);

#include "saltcube_exception.hpp"

#include <csignal>

/** 
 * @brief Handle signals
 * 
 * @throws signal_exception Throw if a signal is found
 * 
 * @param caller Whows calling this function?
 * @param file What file is calling this function?
 * @param line What line is calling this function?
 * 
 * @see __PRETTY_FUNCTION__
 * @see __FILE_NAME__
 * @see __LINE__
 */
void check_signal(const char * caller, std::string file, int line);

/**
 * @brief Setup signal handling
 * 
 */
void configure_signals();

/**
 * @brief Get the signal status
 * 
 * @return sig_atomic_t 
 */
sig_atomic_t get_signal();


extern "C" {
    /**
    * @brief Set a signal
    * 
    */
    void signal_handler(int);
}

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

class signal_exception final : public saltcube_exception {
    int signal;

    public:

    signal_exception(const char * what, const std::string where, int line, int sig);

    int sig() const noexcept;

    const char * full_message() noexcept override;

    private:
    void noinit() override;
};

#endif