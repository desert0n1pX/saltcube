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

bool follow_mode = false;

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


void get_entry_from_file(options &opts, password_handler &passwd_container, std::unique_ptr<entry> &existing_entry, simple_file &savefile) {
    if (opts.get_username().size() > 0) {
        existing_entry = find_entry_by_username(savefile.get_lines(), opts.get_username());
    } else {
        passwd_container.collect();

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
    if (opts.get_nocache()) {
        std::cout << COLOR_FG_YELLOW "Command " COLOR_FG_RED "\"remind\"" COLOR_FG_YELLOW " unavailable, nocache is enabled.\n" COLOR_FG_DEFAULT;

    } else if (passwd_container.is_set()) {
        std::cout << "Type \"" COLOR_FG_GREEN "remind" COLOR_FG_DEFAULT "\" to see your password.\n";

    } else {
        std::cout << "Type \"" COLOR_FG_GREEN "remind" COLOR_FG_DEFAULT "\" to see your password after typing it correctly.\n";
    }
}

void show_follow_status(options &opts, password_handler &passwd_container) {
    if (opts.get_nocache()) {
        std::cout << COLOR_FG_YELLOW "Command " COLOR_FG_RED "\"follow\"" COLOR_FG_YELLOW " unavailable, nocache is enabled.\n" COLOR_FG_DEFAULT;

    } else if (passwd_container.is_set()) {
        std::cout << "Type \"" COLOR_FG_GREEN "follow" COLOR_FG_DEFAULT "\" to see your password interactively.\n";

    } else {
        std::cout << "Type \"" COLOR_FG_GREEN "follow" COLOR_FG_DEFAULT "\" to see your password after typing it correctly.\n";
    }
}

void practice_help(options &opts, password_handler &passwd_container){
    std::cout << "Type \"" COLOR_FG_GREEN "clear" COLOR_FG_DEFAULT "\" to clear the screen.\n"
                 "Type \"" COLOR_FG_GREEN "clear all" COLOR_FG_DEFAULT "\" to clear the screen and screen history.\n"
                 "Type \"" COLOR_FG_GREEN "exit" COLOR_FG_DEFAULT "\" to exit.\n";
                 show_follow_status(opts, passwd_container);
    std::cout << "Type \"" COLOR_FG_GREEN "follow off" COLOR_FG_DEFAULT "\" hide your password when you type.\n"
                 "Type \"" COLOR_FG_GREEN "help" COLOR_FG_DEFAULT "\" to see this page.\n";
                 show_remind_status(opts, passwd_container);
}

void pre_loop_messages() {
  std::cout << "Type \"" COLOR_FG_GREEN "exit" COLOR_FG_DEFAULT "\" to exit.\n"
               "Type \"" COLOR_FG_GREEN "help" COLOR_FG_DEFAULT "\" for a list of in-practice commands.\n";
}

void prompt_pass(password_handler &contianer){
    if (follow_mode) {
        contianer.follow();
    } else {
        contianer.collect();
    }
}

void check_loop(options &opts, password_handler &passwd_container, std::unique_ptr<entry> &existing_entry) {
    pre_loop_messages();

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
        if (opts.get_nocache()) {
            std::cout << COLOR_FG_YELLOW "Unable to use this command, nocache is enabled\n" COLOR_FG_DEFAULT;
        } else if (!passwd_container.is_set()) {
            std::cout << COLOR_FG_YELLOW "Unable to use this command, you must first enter your "
                     "password correctly once\n" COLOR_FG_DEFAULT;
        } else {
            std::cout << COLOR_FG_GREEN "Follow along mode enabled.\n" COLOR_FG_DEFAULT;
            follow_mode = true;
        }
    
    } else if (passwd_container.equals_canidate("follow off")) {
        std::cout << COLOR_FG_GREEN "Follow along mode disabled.\n" COLOR_FG_DEFAULT;
        follow_mode = false;
    
    } else if (passwd_container.equals_canidate("help")) {
        practice_help(opts, passwd_container);
    
    } else if (passwd_container.equals_canidate("remind")) {
      if (opts.get_nocache()) {
        std::cout << COLOR_FG_YELLOW "Unable to use this command, nocache is enabled\n" COLOR_FG_DEFAULT;
      } else if (!passwd_container.is_set()) {
        std::cout << COLOR_FG_YELLOW "Unable to use this command, you must first enter your "
                     "password correctly once\n" COLOR_FG_DEFAULT;
      } else {
        passwd_container.show_password();
      }



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