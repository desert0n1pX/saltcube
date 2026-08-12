/*
SaltCube: A CLI tool to help you remember passwords
Copyright (C) 2026  @desert0n1pX <desert0n1pX ( at) pm [ dot ] me>
*/

#include <exception>
#include <sodium.h>
#include <stdexcept>
#include <string>

#include "logging.hpp"
#include "modes.hpp"
#include "options.hpp"
#include "signal_handler.hpp"
#include "texts.hpp"

/*
Error codes:

0:   Normal operation
1:   Interrupt
2:   File write error
4:   No entry found
5:   Bad argument
6:   Invalid input
7:   Short password
8:   Invalid Entry
9:   File not found
10:  Entry not found
11:  Entry exists
254: Unable to set up signal handler 
 */


int main(int argc, char **argv){
    try {
        options opts;
        configure_signals();
        opts.load(argc, argv);

        switch (opts.get_mode()) {
        case PW_ADD:
            return pw_add(opts);
            break;
        case PW_DEL:
            return pw_del(opts);
            break;
        case PW_HASH:
            return pw_hash(opts);
            break;
        case PW_TEST:
            return pw_test(opts);
            break;
        case HASH_ADD:
            return hash_add(opts);
            break;
        case HASH_DEL:
            return hash_del(opts);
            break;
        case HASH_VER:
            return hash_ver(opts);
            break;
        case EXIT:
            return(0);
            break;
        }
    } catch (show_text_exception &) {
        return 0;

    } catch (signal_exception &except){
        return 1;

    } catch (invalid_cli_argument_error &exception) {
        logging::log(logging::ERROR, "Invalid argument! Use \"-h\" for help or \"-ldebug\" for more.", exception);
        return(5);

    } catch (invalid_input_error &except) {
        logging::log(logging::ERROR, "Invalid input! Use \"-h\" for help or \"-ldebug\" for more", except);
        return(5);

    } catch (std::runtime_error &except) {
        logging::log(logging::ERROR, "An error occured use \"-ldebug\" for more", except);
        return 253;
    } catch (std::exception &except) {
        logging::log(logging::ERROR, "Something really bad happened and an exception was caught that was not expected! Maybe(?) use \"-ldebug\" for more info", except);
        return 254;
    } catch (...) {
        logging::log(logging::ERROR, "Something really bad happened and something (I don't even know what) was caught that was not expected! Maybe(?) use \"-ldebug\" for more info");
        return 255;
    }
}