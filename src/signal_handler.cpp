/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "signal_handler.hpp"

#include "logging.hpp"

#include <string.h>

#include <csignal>
#include <format>
#include <stdexcept>
#include <string>

#define SIG_SETUP_ERR "Unable to configure signal handling, cannot continue!"

volatile sig_atomic_t gSignalStatus = 0;

void check_signal(const char * caller, std::string file, int line){
    if (gSignalStatus == 0)
        return;

    logging::log(logging::NOTICE, "Exiting...");
    throw signal_exception(caller, file, line, gSignalStatus);
}

void configure_signals(){
    
    struct sigaction sigact {};
    sigact.sa_handler = signal_handler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags &= ~SA_RESTART;

    if (sigaction(SIGINT, &sigact, nullptr) != 0) {
        logging::log(logging::ERROR, SIG_SETUP_ERR);
        throw std::runtime_error(SIG_SETUP_ERR);
    } else {
        logging::log(logging::DEBUG, "Signal handling configured");
    }
}

sig_atomic_t get_signal(){
    return gSignalStatus;
}

extern "C" {
    void signal_handler(int sig){
    gSignalStatus = sig;
    return;
    }
}

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

signal_exception::signal_exception(const char * what, const std::string where, int line, int sig) : 
            saltcube_exception(what, where, line, "Signal " + std::to_string(sig) + " aka " + strsignal(sig) + " was caught"){
}

int signal_exception::sig() const noexcept {
    return this->signal;
}

const char * signal_exception::full_message() noexcept {
    full_message_str = std::format("{0} failed in {1} because {2}", what_failed, where_happened, why_happened);

    return full_message_str.c_str();
}

void signal_exception::noinit() {}