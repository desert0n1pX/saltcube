/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "argon2_worker.hpp"
#include "entry.hpp"
#include "logging.hpp"
#include "modes_helper.hpp"
#include "options.hpp"
#include "password_handler.hpp"
#include "sha512_worker.hpp"
#include "simple_file_handler.hpp"
#include "util.hpp"

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum read_mode {
    NORMAL,
    FOLLOW,
    SHADOW
};

enum read_mode read_mode = NORMAL;

password_handler * loop_passwd;

bool check_entry_against_hash(entry &entry, const char * passwd){
    switch (entry.get_mode_enum()) {

    case ARGON2:
        if (worker_argon2_check(entry.get_passwd_hash().c_str(), passwd, strlen(passwd)) == 0) {
            return true;
        } else {
            return false;
        }
    case SHA512:
        if (worker_sha512_check(entry.get_passwd_hash().c_str(), passwd, strlen(passwd)) == 0) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}


std::unique_ptr<entry> find_entry_by_pwhash(std::vector<std::string> &data, password_handler &passwd_container, options &opts){
    std::unique_ptr<entry> line_entry;
    int linenum = 0;
    std::cout << "Looking for your password by hash, this may take a minute...\n";
    for (std::string line : data) {
        
        if (line.size() == 0){
            logging::log(logging::INFO, "line " + std::to_string(linenum) + " is a blank line, skipping");
            continue;
        }

        try {
            line_entry = std::make_unique<entry>(line);
            if (check_entry_against_hash(*line_entry, passwd_container.get_canidate())){
                if (!opts.get_nocache()) {
                    passwd_container.save();
                }
                return line_entry;
            }
        } catch (invalid_entry_exception &except){
            logging::log(logging::ERROR, "Invalid entry: " + line, except);
            continue;

        } catch (key_not_found_error &except){
            logging::log(logging::ERROR, "Invalid entry doesn't contain key for hashing parameters: " + line, except);
            continue;

        } catch (value_improper_format_error &except){
            logging::log(logging::ERROR, "Invalid entry contains improper parameters: " + line, except);
            continue;
        }
    }
    return nullptr;
}

std::unique_ptr<entry> find_entry_by_username(std::vector<std::string> &data, std::string username){
    std::unique_ptr<entry> line_entry;
    int linenum = 0;
    for (std::string line : data) {
        linenum++;
        if (line.size() == 0){
            logging::log(logging::INFO, "line " + std::to_string(linenum) + " is a blank line, skipping");
            continue;
        }

        try {
            line_entry = std::make_unique<entry>(line);
        } catch (invalid_entry_exception &except){
            logging::log(logging::ERROR, "Invalid entry: " + line + " line " + std::to_string(linenum), except);
            continue;
        }

        if (line_entry->get_username() == username)
            return line_entry;
    }
    return nullptr;
}

void delete_entry(simple_file &savefile, entry *entry_from_file){
    std::vector<std::string> &data = savefile.get_lines();
    for (auto pos = data.begin() ; pos != data.end() ; ){
        if (*pos == entry_from_file->get_unparsed()){
            data.erase(pos);
        } else {
            pos++;
        }
    }
    savefile.write_file();
}

void password_accessible_prompt(options &options,
                                password_handler & contianer,
                                std::string disabled_prompt,
                                std::string unavail_prompt,
                                std::string enabled_prompt,
                                void (*on_success)(void)){
    if (options.get_nocache()) {
            std::cout << disabled_prompt << "\n";
        } else if (!contianer.is_set()) {
            std::cout << unavail_prompt << "\n";
        } else {
            std::cout << enabled_prompt << "\n";
            on_success();
        }
}

void get_entry_from_file(options &opts, password_handler &passwd_container, std::unique_ptr<entry> &existing_entry, simple_file &savefile) {
    if (opts.get_username().size() > 0) {
        existing_entry = find_entry_by_username(savefile.get_lines(), opts.get_username());
    } else {
        passwd_container.collect("Enter a password to look for...");

        existing_entry = find_entry_by_pwhash(savefile.get_lines(), passwd_container, opts);
    }
}

void reset_canidate(bool correct_password, password_handler &passwd_container, options &opts) {
    if (correct_password) {
        std::cout << COLOR_FG_GREEN "Password correct.\n" COLOR_FG_DEFAULT;
        if (!opts.get_nocache() && !passwd_container.is_set()) {
            passwd_container.save();
        }
        passwd_container.clear_canidate();
     } else {
        std::cout << COLOR_FG_RED "Password incorrect.\n" COLOR_FG_DEFAULT;
        passwd_container.clear_canidate();
    }
}

void show_remind_status(options &opts, password_handler &passwd_container) {
    password_accessible_prompt(opts, passwd_container, 
                                "Cmnd \"" COLOR_FG_RED "remind" COLOR_FG_DEFAULT "\" unavailable, nocache is enabled",
                                "Type \"" COLOR_FG_YELLOW "remind" COLOR_FG_DEFAULT "\" to see your password, you must first enter your password correctly once",
                                "Type \"" COLOR_FG_GREEN "remind" COLOR_FG_DEFAULT "\" to see your password",
                                [] -> void {});
}

void show_follow_status(options &opts, password_handler &passwd_container) {
    password_accessible_prompt(opts, passwd_container, 
                                "Cmnd \"" COLOR_FG_RED "follow" COLOR_FG_DEFAULT "\" unavailable, nocache is enabled",
                                "Type \"" COLOR_FG_YELLOW "follow" COLOR_FG_DEFAULT "\" to interactively see your password, you must first enter your password correctly once",
                                "Type \"" COLOR_FG_GREEN "follow" COLOR_FG_DEFAULT "\" to interactively see your password",
                                [] -> void {});
}

void show_shadow_status(options &opts, password_handler &passwd_container) {
    password_accessible_prompt(opts, passwd_container, 
                                "Cmnd \"" COLOR_FG_RED "shadow" COLOR_FG_DEFAULT "\" unavailable, nocache is enabled",
                                "Type \"" COLOR_FG_YELLOW "shadow" COLOR_FG_DEFAULT "\" to see correctness, you must first enter your password correctly once",
                                "Type \"" COLOR_FG_GREEN "shadow" COLOR_FG_DEFAULT "\" to  see correctness",
                                [] -> void {});
}

void practice_help(options &opts, password_handler &passwd_container){
    std::cout << "Type \"" COLOR_FG_GREEN "clear" COLOR_FG_DEFAULT "\" to clear the screen.\n"
                 "Type \"" COLOR_FG_GREEN "clear all" COLOR_FG_DEFAULT "\" to clear the screen and screen history.\n"
                 "Type \"" COLOR_FG_GREEN "exit" COLOR_FG_DEFAULT "\" to exit.\n";
                 show_follow_status(opts, passwd_container);
    std::cout << "Type \"" COLOR_FG_GREEN "help" COLOR_FG_DEFAULT "\" to see this page.\n";
    std::cout << "Type \"" COLOR_FG_GREEN "hide" COLOR_FG_DEFAULT "\" to hide input.\n";
                 show_remind_status(opts, passwd_container);
                 show_shadow_status(opts, passwd_container);
}

void pre_loop_messages() {
  std::cout << "Type \"" COLOR_FG_GREEN "exit" COLOR_FG_DEFAULT "\" to exit.\n"
               "Type \"" COLOR_FG_GREEN "help" COLOR_FG_DEFAULT "\" for a list of in-practice commands.\n";
}

void prompt_pass(password_handler &contianer){
    switch (read_mode) {
    case NORMAL:
        contianer.collect("Enter your password or command...");
        break;
    case FOLLOW:
        contianer.follow("Follow along or enter a command...");
        break;
    case SHADOW:
        contianer.follow("Enter your password or command...", ' ');
        break;
    }
}

void check_loop(options &opts, password_handler &passwd_container, std::unique_ptr<entry> &existing_entry) {
    pre_loop_messages();
    loop_passwd = &passwd_container;

    while (true) {
    prompt_pass(passwd_container);

    if (passwd_container.equals_canidate("clear")) {
        std::cout << TERM_ERASE_SCREEN COLOR_FG_GREEN "Current screen cleared\n" COLOR_FG_DEFAULT;

    } else if (passwd_container.equals_canidate("clear all")) {
        std::cout << TERM_ERASE_SCREEN TERM_ERASE_SAVED COLOR_FG_GREEN "Terminal cleared\n" COLOR_FG_DEFAULT;
    
    } else if (passwd_container.equals_canidate("exit")) {
        std::cout << COLOR_FG_GREEN TERM_MOVE_HOME TERM_ERASE_SCREEN TERM_ERASE_SAVED "Terminal cleared before exit.\n" TERM_FORMAT_RESET;
        break;
    
    } else if (passwd_container.equals_canidate("follow")) {
        password_accessible_prompt(opts, passwd_container, 
                                    COLOR_FG_RED "Unable set follow mode, nocache is enabled" COLOR_FG_DEFAULT,
                                    COLOR_FG_YELLOW "Unable set follow mode, you must first enter your password correctly once" COLOR_FG_DEFAULT,
                                    "Follow along mode enabled.",
                                    [] -> void {read_mode = FOLLOW;});
    
    } else if (passwd_container.equals_canidate("help")) {
        practice_help(opts, passwd_container);
    
    } else if (passwd_container.equals_canidate("hide")) {
        std::cout << COLOR_FG_GREEN "Setting hide mode.\n" COLOR_FG_DEFAULT;
        read_mode = NORMAL;
    
    } else if (passwd_container.equals_canidate("shadow")) {
        password_accessible_prompt(opts, passwd_container, 
                                    COLOR_FG_RED "Unable to set shadow mode, nocache is enabled" COLOR_FG_DEFAULT,
                                    COLOR_FG_YELLOW "Unable to set shadow mode, you must first enter your password correctly once" COLOR_FG_DEFAULT,
                                    "Shadow mode enabled.",
                                    [] -> void {read_mode = SHADOW;});
    
    } else if (passwd_container.equals_canidate("remind")) {
        password_accessible_prompt(opts, passwd_container, 
                                    COLOR_FG_RED "Unable to use this command, nocache is enabled" COLOR_FG_DEFAULT,
                                    COLOR_FG_YELLOW "Unable to use this command, you must first enter your password correctly once" COLOR_FG_DEFAULT,
                                    "",
                                    [] -> void {loop_passwd->show_password();});

    } else {
      if (passwd_container.is_set()) {
        reset_canidate(passwd_container.check_password(), passwd_container, opts);

      } else {
        bool hashes_correctly = check_entry_against_hash(*existing_entry, passwd_container.get_canidate());
        reset_canidate(hashes_correctly, passwd_container, opts);
      }

    }
  }
}