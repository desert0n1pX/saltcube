/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "options.hpp"

#include <cstddef>
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include "argon2_worker.hpp"
#include "logging.hpp"
#include "saltcube_exception.hpp"
#include "sha512_worker.hpp"
#include "signal_handler.hpp"
#include "texts.hpp"
#include "util.hpp"

#define MULTI_MODE_ERROR "Can't set multiple modes"
#define UNKNOWN_ARG_ERROR "Unknown argument, use --help for help"
#define NOT_ENOUGH_ARGS_ERROR "Not enough arguments provided"
#define ARG_REDEFINITION_ERROR_FMT "Argument \"{}\" specified to many times"

// Functions for this file only
//////////////////////////////////////////////////////////////////////

void throw_redef(std::string &arg, bool &flag){
    if (flag) throw redefined_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, std::format(ARG_REDEFINITION_ERROR_FMT, arg));
}

size_t to_sizet(const std::string &str_num, size_t starting_pos){
    long long number;
    try {
        number = std::stoll(str_num.substr(starting_pos));
    } catch (std::invalid_argument &) {
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, str_num + " is not a number!");
    }
    if (number < 0)
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, std::format("You provided {0} which is a negative number, please try again with a positive number.", std::to_string(number)));
    
    return (size_t) number;
}

// Public Constructors
//////////////////////////////////////////////////////////////////////

options::options(){
  // Set the defaults
  this->mode = PW_TEST;
  this->mode_touched = false;

  this->algo = "argon2";
  this->algo_touched = false;

  this->file = DEFAULT_FILE;
  this->file_touched = false;

  this->iterations = argon2_pwhash_ITERATIONS_DEFAULT;
  this->iterations_touched = false;

  this->loglevel = DEFAULT_LOG_LEVEL;
  this->loglevel_touched = false;

  this->KiB_memory = argon2_pwhash_MEMORY_DEFAULT;
  this->memory_touched = false;

  this->nocachepasswd = false;
  this->nocachepasswd_touched = false;

  this->parallel = sha512_pwhash_PARALLEL_DEFAULT;
  this->parallel_touched = false;

  this->username = "";
  this->username_touched = false;

  logging::set_loglevel(DEFAULT_LOG_LEVEL);
}

// Public Methods
//////////////////////////////////////////////////////////////////////

void options::enable_nocache(){
    this->nocachepasswd = true;
}

const std::string options::get_algo() const {
    return algo;
}

const std::string options::get_file() const {
    return file;
}

size_t options::get_iterations() const {
    return iterations;
}

size_t options::get_memory_KiB() const {
    return KiB_memory;
}

enum EXEC_MODE options::get_mode() const{
    return mode;
}

bool options::get_nocache() const {
    return nocachepasswd;
}

size_t options::get_parallel() const {
    return parallel;
}

const std::string options::get_username() const {
    return username;
}

void options::load(int &argc, char **argv) {
    for (int count = 1; count < argc; count++) {
        std::string this_arg = argv[count];
        logging::log(logging::DEBUG, "Arg " + std::to_string(count) + " of " + std::to_string(argc - 1) + " is " + this_arg);

        if (this_arg.substr(0, 11) == "--algorithm" || this_arg.substr(0, 2) == "-a") {
            set_opt(argc, argv, count, this_arg, this->algo, this->algo_touched, "algorithm", 'a');
            if ((!this->iterations_touched) && algo == "sha512")
                this->iterations=sha512_pwhash_ITERATIONS_DEFAULT;

        } else if (this_arg.substr(0, 6) == "--file" || this_arg.substr(0, 2) == "-f") {
            set_opt(argc, argv, count, this_arg, this->file, this->file_touched, "file", 'f');

        } else if (this_arg == "help" || this_arg == "-h" || this_arg == "--help") {
            help(argv[0]);

        } else if (this_arg.substr(0, 12) == "--iterations" || this_arg.substr(0, 2) == "-i") {
            set_opt(argc, argv, count, this_arg, this->iterations, this->iterations_touched, "iterations", 'i');
        
        } else if (this_arg.substr(0, 9) == "--license" && this_arg.size() == 9) {
            license();

        } else if (this_arg.substr(0, 14) == "--license-full" && this_arg.size() == 14) {
            license_full();

        } else if (this_arg.substr(0, 12) == "--loglevel" || this_arg.substr(0, 2) == "-l") {
            set_opt(argc, argv, count, this_arg, this->loglevel, this->loglevel_touched, "loglevel", 'l');
            logging::set_loglevel(this->loglevel);

        } else if (this_arg.substr(0, 12) == "--memory" || this_arg.substr(0, 2) == "-m") {
            set_opt(argc, argv, count, this_arg, this->KiB_memory, this->memory_touched, "memory", 'm');
        
        } else if (this_arg.substr(0, 9) == "--nocache" || this_arg.substr(0, 2) == "-n") {
            throw_redef(this_arg, this->nocachepasswd_touched);
            if (this_arg.contains("false"))
                continue;
            this->nocachepasswd = true;

        } else if (this_arg.substr(0, 12) == "--parallel" || this_arg.substr(0, 2) == "-p") {
            set_opt(argc, argv, count, this_arg, this->parallel, this->parallel_touched, "parallel", 'p');

        } else if (this_arg.substr(0, 12) == "--username" || this_arg.substr(0, 2) == "-u") {
            set_opt(argc, argv, count, this_arg, this->username, this->username_touched, "username", 'u');

        } else {
            set_mode(this_arg);
        }
    }

    this->validate();
    this->print_state();
}

void options::print_state() const {
    std::string out = std::format("Current options parsed:\n"
        "this->mode = {}\n"
        "this->mode_touched = {}\n"
        "this->algo = {}\n"
        "this->algo_touched = {}\n"
        "this->file = {}\n"
        "this->file_touched = {}\n"
        "this->iterations = {}\n"
        "this->iterations_touched = {}\n"
        "this->KiB_memory = {}\n"
        "this->memory_touched = {}\n"
        "this->nocachepasswd = {}\n"
        "this->nocachepasswd_touched = {}\n"
        "this->parallel = {}\n"
        "this->parallel_touched = {}\n"
        "this->loglevel = {}\n"
        "this->loglevel_touched = {}\n"
        "this->username = {}\n"
        "this->username_touched = {}\n",

        std::to_string(this->mode),
        this->mode_touched,
        this->algo,
        this->algo_touched,
        this->file,
        this->file_touched,
        this->iterations,
        this->iterations_touched,
        this->KiB_memory,
        this->memory_touched,
        this->nocachepasswd,
        this->nocachepasswd_touched,
        this->parallel,
        this->parallel_touched,
        this->loglevel,
        this->loglevel_touched,
        this->username,
        this->username_touched
    );
    logging::log(logging::DEBUG, out);
}

bool options::prompt_for_username() {
    if (username.size() == 0) {
        std::cout << "Enter a username or press enter to continue without one:\n";
        std::getline(std::cin, username);
        SIGCHECK
        if (username.size() > 0) {
            try {
                this->validate();
            } catch (invalid_cli_argument_error &except) {
                throw invalid_input_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "Invalid Username, use -h for help"); 
            }
            return true;
        }
        return false;
    }

    return true;
}

void options::validate() {
    if (!(this->algo == "argon2" || this->algo == "sha512"))
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, this->algo + " is not a valid hash algorithm");
    
    if (this->file.size() == 0) 
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "filename is empty");

    if ((this->algo == "argon2" && (this->iterations < 1 || this->iterations > argon2_pwhash_ITERATIONS_MAX)) ||
        (this->algo == "sha512" && (this->iterations < sha512_pwhash_ITERATIONS_MIN || this->iterations > sha512_pwhash_ITERATIONS_MAX))) // 2^27 for sha512
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "number of iterations out of range");

    if (this->KiB_memory < argon2_pwhash_MEMORY_MIN) 
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "Not enough memory allocated for argon2");
    
    if (this->KiB_memory > get_avail_mem_in_KiB()) 
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, std::to_string(KiB_memory) + " KiB of memory is greater that the total system memory");

    if (!has_in_string(this->loglevel, {"error", "critical", "warning", "notice", "info", "debug"}))
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, this->loglevel + " is not a valid loglevel, use -h for more info");

    size_t supported_threads = std::thread::hardware_concurrency();
    supported_threads = (supported_threads > sha512_pwhash_PARALLEL_MAX_PREF) ? sha512_pwhash_PARALLEL_MAX_PREF : supported_threads;
    logging::log(logging::DEBUG, std::to_string(supported_threads) + " threads are supported.");
    if (this->algo == "sha512" && (this->parallel < 1 || this->parallel > supported_threads))
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "parallel option out of range, " + std::to_string(supported_threads) + " threads supported");

    if (has_in_string(this->username, {"$",","}))
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, "Username invalid, cannot include characters: \"$\", \",\"");
}

// Private Methods
//////////////////////////////////////////////////////////////////////

void options::set_mode(const std::string &mode){
    if (this->mode_touched && (mode == "pw-add" || mode == "pw-del" || mode == "pw-hash" || mode == "pw-test" || mode == "hash-add" || mode == "hash-del" || mode == "hash-ver") ) {
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, MULTI_MODE_ERROR);

    } else if (mode == "pw-add") {
        this->mode = PW_ADD;

    } else if (mode == "pw-del") {
        this->mode = PW_DEL;

    } else if (mode == "pw-hash") {
        this->mode = PW_HASH;

    } else if (mode == "pw-test") {
        this->mode = PW_TEST;

    } else if (mode == "hash-add") {
        this->mode = HASH_ADD;

    } else if (mode == "hash-del"){
        this->mode = HASH_DEL;

    } else if (mode == "hash-ver"){
        this->mode = HASH_VER;

    } else {
        throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, mode + " is not a known mode");
    }
    this->mode_touched = true;
}

void options::set_opt(int &argc, char **argv, int &count, std::string &this_arg, size_t &field, bool &set, const std::string longopt, const char shortopt) {
    throw_redef(this_arg, set);
    if (this_arg.starts_with(std::format("--{0}=", longopt))) { // --file=<arg>
        field = to_sizet(this_arg, longopt.size() + 3);
        logging::log(logging::DEBUG, std::format("size_t -> Set --{0}=<{1}>", longopt, this_arg.substr(longopt.size() + 3)));

    } else if (this_arg.starts_with(std::format("-{0}=", shortopt))) { // -f=<arg>
        field = to_sizet(this_arg, 3);
        logging::log(logging::DEBUG, std::format("size_t -> Set -{0}=<{1}>", shortopt, this_arg.substr(3)));

    } else if (this_arg.starts_with(std::format("-{0}", shortopt)) && this_arg.size() > 2) { // -f<arg>
        field = to_sizet(this_arg, 2);
        logging::log(logging::DEBUG, std::format("size_t -> Set -{0}<{1}>", shortopt, this_arg.substr(2)));

    } else { // --file <arg> or  -f <arg>
        if (++count >= argc){
            throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, NOT_ENOUGH_ARGS_ERROR);
        }
        
        field = to_sizet(argv[count], 0);
        logging::log(logging::DEBUG, std::format("size_t -> Set -{0} or --{1} <{2}>", shortopt, longopt, argv[count]));
    }
    set = true;
}

void options::set_opt(int &argc, char **argv, int &count, std::string &this_arg, std::string &field, bool &set, const std::string longopt, const char shortopt) {
    throw_redef(this_arg, set);
    if (this_arg.starts_with(std::format("--{0}=", longopt))) { // --file=<arg>
        field = this_arg.substr(longopt.size() + 3);
        logging::log(logging::DEBUG, std::format("std::string -> Set --{0}=<{1}>", longopt, this_arg.substr(longopt.size() + 3)));

    } else if (this_arg.starts_with(std::format("-{0}=", shortopt))) { // -f=<arg>
        field = this_arg.substr(3);
        logging::log(logging::DEBUG, std::format("std::string -> Set -{0}=<{1}>", shortopt, this_arg.substr(3)));

    } else if (this_arg.starts_with(std::format("-{0}", shortopt)) && this_arg.size() > 2) { // -f<arg>
        field = this_arg.substr(2);
        logging::log(logging::DEBUG, std::format("std::string -> Set -{0}<{1}>", shortopt, this_arg.substr(2)));

    } else { // --file <arg> or  -f <arg>
        if (++count >= argc){
            throw invalid_cli_argument_error(__PRETTY_FUNCTION__, __FILE_NAME__, __LINE__, NOT_ENOUGH_ARGS_ERROR);
        }

        field = argv[count];
        logging::log(logging::DEBUG, std::format("std::string -> Set -{0} or --{1} <{2}>", shortopt, longopt, argv[count]));
    }
    set = true;
}

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

invalid_cli_argument_error::invalid_cli_argument_error(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why){}

void invalid_cli_argument_error::noinit(){}

//////////////////////////////////////////////////////////////////////

invalid_input_error::invalid_input_error(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why){}

void invalid_input_error::noinit(){}

//////////////////////////////////////////////////////////////////////

redefined_argument_error::redefined_argument_error(const char * what, const std::string where, int line, const std::string why) : 
            saltcube_exception(what, where, line, why){
}

void redefined_argument_error::noinit(){}