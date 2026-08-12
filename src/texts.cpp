/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "texts.hpp"
#include "TEXT_BLOCKS.hpp"
#include "options.hpp"

#include <iostream>
#include <string>

// This file contains the following references large lines of text from TEXT_BLOCKS.hpp:
// const char * help_text;
// const char * license_text;

void help(const char * const binary_name){
    std::string manpage = std::format(help_text, binary_name, DEFAULT_FILE);
    std::cout << manpage;

    throw show_text_exception(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "help page shown, terminate execution");
}

void license(){
    std::cout << license_text;

    throw show_text_exception(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "License page shown, terminate execution");
}

void license_full(){
    std::cout << license_text_full;

    throw show_text_exception(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "Licenses shown, terminate execution");
}

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

show_text_exception::show_text_exception(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why){}

void show_text_exception::noinit(){}
