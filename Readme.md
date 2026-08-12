# ![SaltCube](art/saltcube_wide.png)

SaltCube is a CLI tool designed to help users remember passwords through repetition. SaltCube can store passwords securely by taking the cryptographic hash of your passwords. You can also optionally include a username to associate with the password.

# ![SaltCube](art/gif/github-dark-saltcube.gif)

By default, SaltCube uses the argon2 algorithm (argon2id specifically) to securely hash your passwords. Argon2 won the 2015 password hashing competition as the way the algorithm accesses large memory regions makes it very computationally expensive to brute force.

The alternative algorithm available through this program is sha512. Although it is not memory-hard like argon2, here it is implemented with salting, a default large number of iterations, and parallel computation. Salting makes your password hashes less vulnerable to rainbow tables while iteration and parallelization once again makes brute force attacks more difficult. This is covered more in the
[Making SHA512 Multithreaded Section](#making-a-multithreaded-password-hasher).

## Features

This software is able to: 
 - Hash passwords with sha512 or argon2
 - Verify password hashes
 - Save hashed passwords
 - Train users on passwords

## Getting Started

To use this tool you can either build it from source, download the binary, or if you use arch, you can download the premade package.

### Installing the Pre-made Package

To install the premade package, locate the release you want to install on the releases page and run as root `pacman -U https://<download-url>.tar.zst` or download it and run `pacman -U /path/to/saltcube-<version>-x86_64.pkg.tar.zst`.

### Building From Source

To automatically build and install this project you can run the command `make install` as root in root directory of this project. If you only want to build the project you can just do `make`.

### Using the PKGBUILD

If you are on an archlinux based system, you can build and install this software by downloading the `PKGBUILD` file and running `makepkg -si` in the same directory. Note you should not run this command as root.

## Usage:

```
Usage: saltcube [subcommand] [options...]
Options:
 -a, --algorithm  <algorithm>  Use <algorithm> to calculate hashes when creating
                               password entries, choose from: argon2 or sha512.
                               Default is argon2
 -f, --file       <filename>   Use <filename> to save and look up hashes default
                               is hashes.txt
 -h, --help                    Show this help
 -i, --iterations <number>     Rehash the password <number> times when creating
                               password entries
     --license                 Show the licenses for this software.
     --license-full            Show the full license text, you may want to pipe
                               the output into a pager
 -l, --loglevel   <level>      Print messages of a certain importance or greater
                               Default is NOTICE. Choose from:
                               error, critical, warning, notice, info, debug
 -m, --memory     <KiB>        Restrict argon2 to <KiB> kibibytes of memory when
 -n, --nocache                 Don't keep the password in memory longer than
                               needed. This will make the program rehash the
                               password everytime you enter it in practice mode
                               creating password entries
 -p, --parallel   <number>     Use number threads for mode sha512
 -u, --username   <username>   Assosiate the password with a username or look
                               for a hash already assosiated with a username.
                               Username cannot contain \"$\".\n
Subcommands:
 pw-add                        Hash a password and save it
 pw-del                        Remove a password from the savefile
 pw-hash                       Find the hash assosiated with a password or
                               make a new hash if an existing one isn't found
 pw-test                       Practice typing your password (default option)
 hash-add                      Save an existing hash
 hash-del                      Remove an existing hash, if -u is supplied the
                               hash doesn't have to be provided
 hash-ver                      Enter a hash into the console and test if the
                               password is correct
```

## Making A Multithreaded Password Hasher

As both an attempt to make SHA512 more resilient, and as an opportunity to learn about threads, I decided to make my password hashing implementation multithreaded. I should start off by talking about how fast SHA512 is, which is bad in this case:

```
❯ echo -e "\ntest" | time ./saltcube pw-hash -a sha512 -p 1 -i 1000000
Enter a username or press enter to continue without one:
Enter a password. Characters are hidden.
Looking for your password by hash, this may take a minute...
Creating hash...
New hash with random salt: $sha512$i=1000000,p=1$PodHcZjQeCAGUJAoMPPVMA$Ek/4xoDxbYOvaemWRYpqjNZm08jt6vQwFvBcmcLCNhErQQmfuzLiSwvcoZSqr7n6HqtS78WqfIhHEU7Kif5Kdw
./saltcube pw-hash -a sha512 -p 1 -i 1000000  0.55s user 0.01s system 99% cpu 0.563 total
```

In half a second, my device was able to compute 1,000,000 iterations of SHA512. My initial idea was to, for every iteration, dispatch a set of threads to do some hashing. After joining the threads together I would somehow combine all the data together and take the hash of that. There are 2 big problems with this:

 - If each thread started with the same input, then the output of each would be easily the same and one could simply repeat the output of a single thread back to back and take the hash of that to produce the same result.
  - SHA512 is fast and all the threads will likely finish at different times, meaning some threads are doing nothing between iterations. This would be a bottleneck for something that should easily be able to heavily utilize your CPU.
  
  So I needed solutions to:

  - Supply unique data to every thread
  - Allow each thread to run concurrently with the others for as long as possible without interruption.

### The Solution as Implemented

In this software a new SHA512 hash is generated in the following steps:

- A salt is generated of random bytes
- The salt is hashed `p` times where `p = number of threads`. Each iteration of the SHA512 digest is saved into an array for use by each thread.
- For each thread spawned a different iteration of the salt's digest is used as a password salt
- Each thread does the following in a loop of `i = iterations` times:
    - Combine the password or the result of the last digest with one of the digests of the salt
    - Take the digest of the combined data
- After all the threads finish all their iterations, the digests are all concatenated in the same order that the salt was digested
- A final digest is taken of this total
- The digest and salt are encoded in base64 then combined with the parameters used to generate a string of text

To verify a password hash, the same process occurs, but the salt comes from the decoded base64 substring in the hash-string text. When above process is done, the full final string is compared to the provided string.
