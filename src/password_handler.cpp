/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "password_handler.hpp"

#include <cstring>
#include <iostream>
#include <ostream>
#include <sodium.h>
#include <termios.h>

#include "cube_memory.hpp"
#include "signal_handler.hpp"
#include "logging.hpp"


#define SODIUM_ERROR "Unable to initialize sodium"
#define MEM_REALLOC_ERROR "An error occured when allocating memory!"
#define DEFUALT_MEM 8

// Functions for this file only
//////////////////////////////////////////////////////////////////////

void show_mirror(const char * passwd, const char * current_line, char fill){
    std::cout << TERM_MOVE_UP(1) << "\r";
    for (size_t idx = 0 ; idx < strlen(passwd) ; idx++){
        // When the user typed string is shorter than passwd
        if (idx < strlen(current_line)) {
            if (current_line[idx] == passwd[idx]) {
                std::cout << COLOR_BG_GREEN
                          << ((fill == '\0') ? passwd[idx] : fill) ;
            } else {
                std::cout << COLOR_BG_RED
                          << ((fill == '\0') ? passwd[idx] : fill) ;
            
            }
            
        // When we are still indexing in the password but past what was typed
        } else {
            std::cout << COLOR_BG_DEFAULT TERM_FORMAT_DIM << ((fill == '\0') ? passwd[idx] : fill) ;
        }
    }
     
    // When the user types more than the passwd
    long difference = static_cast<long>(strlen(current_line)) - static_cast<long>(strlen(passwd));
    if (difference > 0) {
        for (long idx = 0 ; idx < difference ; idx++)
            std::cout << COLOR_BG_RED << ' ';
    }

    std::cout << TERM_FORMAT_RESET TERM_ERASE_LINE_REMAIN;
    std::cout << "\n" << current_line << TERM_ERASE_AFTER_CURSOR;
}

// Static Functions
//////////////////////////////////////////////////////////////////////

/**
 * @brief Disable terminal echo while typing
 * 
 */
void password_handler::disable_echo(){
    struct termios termsettings;

    tcgetattr(fileno(stdin), &termsettings); // Get current settings
    termsettings.c_lflag &= ~ECHO; // Set the echo bit to 0 in local attributes
    tcsetattr(fileno(stdin), TCSANOW, &termsettings); // Apply NOW
}

/**
 * @brief Enable terminal echo while typing
 * 
 */
void password_handler::enable_echo(){
    struct termios termsettings;

    tcgetattr(fileno(stdin), &termsettings); // Get current settings
    termsettings.c_lflag |= ECHO; // Set the echo bit to 1 in local attributes
    tcsetattr(fileno(stdin), TCSANOW, &termsettings); // Apply NOW
}

/**
 * @brief Set cannonical mode
 * 
 */
void password_handler::set_nonconnonical(){
    struct termios termsettings;

    tcgetattr(fileno(stdin), &termsettings); // Get current settings
    termsettings.c_lflag &= ~ICANON; // Set the echo bit to 1 in local attributes
    tcsetattr(fileno(stdin), TCSANOW, &termsettings); // Apply NOW
}


/**
 * @brief Set cannonical mode
 * 
 */
void password_handler::set_connonical(){
    struct termios termsettings;

    tcgetattr(fileno(stdin), &termsettings); // Get current settings
    termsettings.c_lflag |= ICANON; // Set the echo bit to 1 in local attributes
    tcsetattr(fileno(stdin), TCSANOW, &termsettings); // Apply NOW
}

// Public Constructors
//////////////////////////////////////////////////////////////////////

password_handler::password_handler() : password(cube_mem_secure(DEFUALT_MEM)), canidate(DEFUALT_MEM) { }

// Public Methods
//////////////////////////////////////////////////////////////////////

bool password_handler::check_password() const {
    return strcmp(canidate.get_as_char(), password.get_as_char()) == 0;
}

void password_handler::clear_canidate(){
    canidate.zero();
}

void password_handler::clear_password(){
    password.zero();
}

void password_handler::collect(std::string prompt){
    disable_echo();
    std::cout << prompt << " Input is " TERM_FORMAT_DIM "hidden...\n" TERM_FORMAT_RESET;

    size_t input_len = 0;
    for (char input = std::getchar() ; !(input == '\0' || input == '\n' || input == EOF) ; input = std::getchar()){
        if (input == '\177') { // backspace(del)
            if (input_len > 0)
                canidate.get_as_char()[--input_len] = '\0';
            continue;
        }

        if (std::strlen(canidate.get_as_char()) + 2 > canidate.size())
            canidate.grow();

        canidate.get_as_char()[input_len] = input;
        canidate.get_as_char()[++input_len] = '\0'; 
    }
    std::cout << "\n";

    SIGCHECK
    enable_echo();
}

bool password_handler::equals_canidate(const char * test_str) const {
    return strcmp(canidate.get_as_char(), test_str) == 0;
}

void password_handler::follow(std::string prompt, char fill){
    disable_echo();
    set_nonconnonical();

    std::cout << prompt << std::endl;
    canidate.zero();
    show_mirror(password.get_as_char(), canidate.get_as_char(), fill);

    size_t input_len = 0;
    for (char input = std::getchar() ; !(input == '\0' || input == '\n' || input == EOF) ; input = std::getchar()){
        if (input == '\177') { // backspace(del)
            if (input_len > 0)
                canidate.get_as_char()[--input_len] = '\0';
            show_mirror(password.get_as_char(), canidate.get_as_char(), fill);
            continue;
        }

        if (std::strlen(canidate.get_as_char()) + 2 > canidate.size())
            canidate.grow();

        canidate.get_as_char()[input_len] = input;
        canidate.get_as_char()[++input_len] = '\0';
        show_mirror(password.get_as_char(), canidate.get_as_char(), fill);
    }
    std::cout << "\n";

    SIGCHECK
    set_connonical();
    enable_echo();
}

const char * password_handler::get_canidate() const {
    return canidate.get_as_char();
}

bool password_handler::is_set() const{
    return strlen(password.get_as_char()) > 0;
}

bool password_handler::save(){
    logging::log(logging::NOTICE, "Password cached in memory. Don't leave this program unattended.");
    if (strlen(password.get_as_char()) == 0) {
        while (password.size() < canidate.size()){
            password.grow(canidate.size());
        }

        strcpy(password.get_as_char(), canidate.get_as_char());
        canidate.zero();
        return true;
    } else {
        return false;
    }
}

void password_handler::show_password() const {
    std::cout << "Your password is \"" COLOR_FG_GREEN << password.get_as_char() << COLOR_FG_DEFAULT "\"\n";
}