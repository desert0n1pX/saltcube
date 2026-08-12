/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include "modes.hpp"
#include "entry.hpp"
#include "logging.hpp"
#include "options.hpp"
#include "password_handler.hpp"
#include "modes_helper.hpp"
#include "signal_handler.hpp"
#include "simple_file_handler.hpp" 
#include "worker_exceptions.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>


password_handler passwd_container;
std::unique_ptr<entry> existing_entry;

int pw_add(options &opts) {
    opts.prompt_for_username();
    simple_file savefile = simple_file(opts.get_file());
    opts.enable_nocache();

    try {
        savefile.load_file();
        get_entry_from_file(opts, passwd_container, existing_entry, savefile);
    } catch (file_not_found &except) {
        logging::log(logging::NOTICE, "File not found, creating one", except);
    }

    if (existing_entry != nullptr) {
        std::cout << "Entry exists.\n";
        return 11;
    } else {
        if (strlen(passwd_container.get_canidate()) == 0)
            passwd_container.collect();

        try {
            std::cout << "Creating hash...\n";
            entry new_entry = create_entry_from_password(opts, passwd_container.get_canidate());
            savefile.get_lines().push_back(new_entry.get_entry());
            savefile.write_file();
            std::cout << "Hash saved...\n";

        } catch (file_write_error &except) {
            logging::log(logging::ERROR, "Unable to save file", except);
            return 2;
            
        } catch (worker_hash_error &except) {
            logging::log(logging::ERROR, "Unable to create hash, the worker failed, maybe try different memory or iteration parameters", except);
            return 7;
        }
    }
    return 0;
}

int pw_del(options &opts){
    opts.prompt_for_username();
    simple_file savefile = simple_file(opts.get_file());

    try {
        savefile.load_file(); 
        get_entry_from_file(opts, passwd_container, existing_entry, savefile);
    } catch (file_not_found &) {
        logging::log(logging::ERROR, "File not found, can't continue");
        return 9;
    }

    if (existing_entry == nullptr) {
        logging::log(logging::ERROR, "Entry not found, can't continue");
        return 10;

    } else {
        try {
            delete_entry(savefile, existing_entry.get());
        } catch (file_write_error &) {
            return 2;
        }
    }
    
    return 0;
}

int pw_hash(options &opts){
    opts.prompt_for_username();
    simple_file savefile = simple_file(opts.get_file());

    try { 
        savefile.load_file();
        get_entry_from_file(opts, passwd_container, existing_entry, savefile);
    } catch (file_not_found &except) {
        logging::log(logging::WARNING, "File not found, unable to check for duplicates", except);
    }

    if (existing_entry != nullptr) {
        std::cout << "Entry found, not creating a new one.\n"
                     "The existing entry is: " << existing_entry->get_entry() << std::endl;
        return 11;

    } else {
        if (strlen(passwd_container.get_canidate()) == 0)
            passwd_container.collect();

        try {
            std::cout << "Creating hash...\n";
            entry new_entry = create_entry_from_password(opts, passwd_container.get_canidate());
            std::cout << "New hash with random salt: " + new_entry.get_entry() + "\n";
            
        }  catch (worker_hash_error & except) {
            logging::log(logging::ERROR, "Unable to create hash, the worker failed, maybe try different memory or iteration parameters", except);
            return 7;
        }
    }
    return 0;
}


int pw_test(options &opts) {
    opts.prompt_for_username();
    simple_file savefile = simple_file(opts.get_file());

    try { 
        savefile.load_file();
        get_entry_from_file(opts, passwd_container, existing_entry, savefile);

    } catch (file_not_found &except) {
        logging::log(logging::ERROR, "File not found, can't continue", except);
        return  9;
    }

    if (existing_entry == nullptr) {
        logging::log(logging::ERROR, "No entry found! Can't continue");
        return 10;

    } else {
        check_loop(opts, passwd_container, existing_entry);
    }
    return 0;
}

int hash_add(options &opts){
    std::string new_hashstr;
    simple_file savefile = simple_file(opts.get_file());

    try {
        savefile.load_file();
    } catch (file_not_found &except) {
        logging::log(logging::WARNING, "File not found, one will be created", except);
    }

    std::cout << "Please enter a hash string.\n >>> ";
    std::getline(std::cin, new_hashstr);
    SIGCHECK
    
    try {
        entry new_entry = entry(new_hashstr);
        savefile.get_lines().push_back(new_entry.get_entry());
        savefile.write_file();
        
    } catch (file_write_error &except) {
        logging::log(logging::ERROR, "Error while writing file!", except);
        return 2;

    } catch (invalid_entry_exception &except) {
        logging::log(logging::ERROR, "The hash you entered is invalid", except);
        return 8;
    }

    return 0;
}

int hash_del(options &opts){
    std::string entry_to_delete;
    simple_file savefile = simple_file(opts.get_file());

    try {
        savefile.load_file();
    } catch (file_not_found &except) {
        logging::log(logging::ERROR, "File not found. Can't continue.", except);
        return 9;
    }

    std::cout << "Please paste the entry to remove. No parsing is done so the entry must extactly match a line in the file.\n >>> ";
    std::getline(std::cin, entry_to_delete);
    SIGCHECK

    std::vector<std::string> &data = savefile.get_lines();
    auto itr_pos = std::find(data.begin(), data.end(), entry_to_delete);

    if (itr_pos == data.end()) {
        logging::log(logging::ERROR, "Entry not found! Can't continue.");
        return 10;

    } else {
        logging::log(logging::INFO, "Removing entr(ies)");
        for (auto pos = data.begin() ; pos != data.end() ; ){
            if (*pos == entry_to_delete){
                logging::log(logging::INFO, "Erasing an entry");
                data.erase(pos);
            } else {
                pos++;
            }
        }
        
        try {
            savefile.write_file();
        } catch (file_write_error &except) {
            logging::log(logging::ERROR, "Unable to write to file", except);
            return 2;
        }
    }
    
    return 0;
}

int hash_ver(options &opts){
    std::string entry_stdin;

    std::cout << "Enter a hash to test against:\n >>> ";
    getline(std::cin, entry_stdin);

    try { 
        existing_entry = std::make_unique<entry>(entry_stdin);
        check_loop(opts, passwd_container, existing_entry);

    } catch (invalid_entry_exception &except) {
        logging::log(logging::ERROR, "Invalid hash, can't continue", except);
        return  9;
    } catch (worker_hash_error &except) {
        logging::log(logging::ERROR, "Invalid hash, worker failed", except);
        return  8;
    }
    return 0;
}