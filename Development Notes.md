# Memory and Secure Memory

Avoid `malloc()` at all costs. Using it plainly in this code base is sure to go awfully given how many exceptions are thrown all over the place. Instead, use `cube_mem(size_t)` or `cube_mem_secure(size_t)` if secure memory is needed, such as that to store passwords.

`cube_mem` and its child classes are RAII so they are safe in case of exceptions.

# Exceptions

Speaking of exceptions THERE ARE A TON. As a person that learned Java as their first major language, I found C++ to be lacking in available exceptions to throw as well as the information they provide (`.what()` kinda sucks). This project adds a base class for exceptions to create different categories of exceptions for different errors. 

Every derived exception should be defined in the same file that is most appropriate to its type (such that something like signal_exception is defined in `signal_handler.cpp` and not `logging.cpp`).

The base class also provides more functions than just `what()`:
 - `saltcube_exception::what()`: The normal C++ style
 - `saltcube_exception::where()`: Where was the exception thrown
 - `saltcube_exception::why()`: Why was the exception thrown, what did we define as the error?
 - `saltcube_exception::full_message()`: A full sentence constructed from the information.

Most exceptions, especially custom ones, should be handled in `mode.cpp` or `mode_helper.cpp`.

 # Signal Handling

 This project sets up a signal handler. All it does is set a flag in a `volatile sig_atomic_t`. This flag should be checked when safe after input cases like `std::getline()` and `std::getchar` as well as after long-running operations like calls to `crypto_pw_hash()`. To check just use the following macro anywhere (after adding `#include "signal_handler.hpp"` of course):

 ```c++
 SIGCHECK
 ```

 If a signal is waiting then this call will throw a `signal_exception`.

 # Logging
 
 There is a set of logging tools written for this project. The default loglevel is `notice` and is set in the constructor of options in `options.cpp`.

 # Defaults

 All default/implicit arguments/options are set in the constructor `options::options()` in `options.cpp`.

 # Debugging And Environment

 This project was developed in a vscode based environment (VSCodium) with `clangd` for code completion. For similar environments you can set up to work and debug this project with the `configure` make target (`make configure`). Note that the tool [Bear](https://github.com/rizsotto/bear) is required to generate the `compile_commands.json`.

 ## Adding debug symbols

 To compile with debug symbols just run the make target you use to build SaltCube with the environment variable `CXX_EXTRA_FLAGS=-g`.

 ## Using a container to build

 SaltCube can be built in docker or podman. If you use docker you must either run make as root or your user must be part of the `docker` group. There are targets for both, so you can either use `in-docker` or `in-podman` as the make target. If you need to pass more flags to `make` just use the `EXTRA_MAKE_FLAGS` environment variable.

 ## VSCode build task

 If you need to change how you build the project for debugging, just edit the `tasks.json` in `.vscode`, it was copied there when you ran `make configure`. Please don't edit `development/launch.json` or `development/tasks.json` unless you plan to make upstream changes.