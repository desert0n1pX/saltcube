/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#ifndef OPTIONS_H
#define OPTIONS_H

#define DEFAULT_FILE std::format("{0}/{1}", getenv("HOME"), ".hashes.txt")

#include "saltcube_exception.hpp"
#include <cstddef>
#include <string> // Used in argument

/**
 * @brief Mode to operate in
 * 
 */
enum EXEC_MODE {
    PW_ADD,
    PW_DEL,
    PW_HASH,
    PW_TEST,
    HASH_ADD,
    HASH_DEL,
    HASH_VER,
    EXIT
};

/**
 * @brief Options for execution
 * 
 */
class options {
    
    // Private Fields
    //////////////////////////////////////////////////////////////////////

    enum EXEC_MODE mode;
    bool           mode_touched;

    std::string algo;
    bool        algo_touched;

    std::string file;
    bool        file_touched;

    size_t      iterations;
    bool        iterations_touched;

    size_t      KiB_memory;
    bool        memory_touched;

    std::string loglevel;
    bool        loglevel_touched;
    
    bool        nocachepasswd;
    bool        nocachepasswd_touched;
    size_t      parallel;
    bool        parallel_touched;

    std::string username;
    bool        username_touched;

    public:

    // Public Constructors
    //////////////////////////////////////////////////////////////////////

    options();

    // Public Destructors
    //////////////////////////////////////////////////////////////////////

    ~options()=default;

    // Public Methods
    //////////////////////////////////////////////////////////////////////

    /**
     * @brief Disable password cacheing
     * 
     */
    void enable_nocache();

    /**
     * @brief Get the algorithm string
     * 
     * @return std::string 
     */
    const std::string get_algo() const;

    /**
     * @brief Get the filename
     * 
     * @return std::string 
     */
    const std::string get_file() const;

    /**
     * @brief get the number of iterations
     * 
     * @return size_t 
     */
    size_t get_iterations() const;

    /**
     * @brief Get the memory for argon2 in KiB
     * 
     * @return size_t 
     */
    size_t get_memory_KiB() const;

    /**
     * @brief Get the mode enum
     * 
     * @return enum EXEC_MODE 
     */
    enum EXEC_MODE get_mode() const;

    /**
     * @brief is nocache set
     * 
     * @return true 
     * @return false 
     */
    bool get_nocache() const;

    /**
     * @brief Get the parallel threads to run
     * 
     * @return size_t 
     */
    size_t get_parallel() const;

    /**
     * @brief Get the desired username
     * 
     * @return const std::string 
     */
    const std::string get_username() const;

    /**
     * @brief Load CLI options
     * 
     * @param argc Number of options
     * @param argv Character array array
     */
    void load(int &argc, char **argv);

    /**
     * @brief Print state to debug log
     * 
     */
    void print_state() const;

    /**
     * @brief Return true if a username is set or if one is entered when prompted
     * 
     * @return true Username entered
     * @return false None entered
     */
    bool prompt_for_username();

    /**
     * @brief Validate the options
     * 
     * @throws invalid_cli_argument_error Bad option
     *
     */
    void validate();

    // Private Methods
    //////////////////////////////////////////////////////////////////////

    private:

    /**
     * @brief Set mode, throw error if called multiple times
     * 
     * @param mode mode string
     */
    void set_mode(const std::string &mode);

    /**
     * @brief Set option based on specific arg
     * 
     * @param argc Number of total args
     * @param argv Character array array
     * @param count Current index
     * @param this_arg Current Argument
     * @param field Field to modify
     * @param set Field's boolean to set
     * @param longopt Long option name
     * @param shortopt Short option name
     */
    void set_opt(int &argc, char **argv, int &count, std::string &this_arg, size_t &field, bool &set, const std::string longopt, const char shortopt);
    void set_opt(int &argc, char **argv, int &count, std::string &this_arg, std::string &field, bool &set, const std::string longopt, const char shortopt);
    
    /**
     * @brief Set loglevel from options
     * 
     */
   // void set_loglevel_from_saved_string();

};

//////////////////////////////////////////////////////////////////////
// Exceptions
//////////////////////////////////////////////////////////////////////

class invalid_cli_argument_error final : public saltcube_exception {
    void noinit() override;

    public:
    invalid_cli_argument_error(const char * what, const std::string where, int line, const std::string why);
};

class invalid_input_error final : public saltcube_exception {
    void noinit() override;

    public:
    invalid_input_error(const char * what, const std::string where, int line, const std::string why);
};

class redefined_argument_error final : public saltcube_exception {
    void noinit() override;

    public:
    redefined_argument_error(const char * what, const std::string where, int line, const std::string why);
};

#endif