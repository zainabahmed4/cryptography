# Cryptography: Caesar and Substitution Ciphers

This project implements encryption and decryption tools for Caesar and Substitution ciphers in C++. It provides command-line utilities for encoding, decoding, and analyzing text using classic cryptographic techniques.

## Features

- **Caesar Cipher**: Encode and decode text with a shift cipher.
- **Substitution Cipher**: Encode and decode text with a random or user-supplied substitution.
- **Englishness Scoring**: Uses quadgram statistics to score how "English-like" a decoded message is.
- **Automated Decryption**: Hill-climbing algorithm to break substitution ciphers.
- **Unit Tests**: Comprehensive tests using GoogleTest.


## Building

Requires `clang++` and [GoogleTest](https://github.com/google/googletest).

```sh
make ciphers_main      # Build the main program
make ciphers_tests     # Build the test suite
```

## Running
- **Main program:**  
./ciphers_main

- **Run all tests:**  
make test_all

- **Run specific tests:**  
make test_caesar_enc  
make test_caesar_dec  
make test_subst_enc  
make test_subst_dec  
make test_subst_dec_file  

## Usage
- The program will prompt you for commands to encrypt, decrypt, or analyze text.  
- Refer to the in-program menu for available options.
