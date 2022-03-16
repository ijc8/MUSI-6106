#ifndef _ENGINE_BRIDGE_H
#define _ENGINE_BRIDGE_H

#include <iostream>
#include <regex>
#include <vector>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: Move this header to third-party folder.
#include "subprocess.h"

#include "GameState.h"

// TODO: Move implementations to EngineBridge.cpp,
// move EngineBridge.cpp:main to test file.

// Concise way to split a string in C++ >= 11. (Based on https://stackoverflow.com/a/64886763/13204291.)
std::vector<std::string> split(const std::string &str, const std::regex &regex) {
    return std::vector<std::string>(std::sregex_token_iterator(str.begin(), str.end(), regex, -1), std::sregex_token_iterator());
}

// TODO: Consider using C++20 (we're using C++17 now) to get this for free.
bool starts_with(const std::string &str, const std::string &prefix) {
    return str.rfind(prefix, 0) == 0;
}

class Subprocess {
public:
    Subprocess(const std::string &path) {
        const char *command_line[] = {path.c_str(), NULL};
        int result = subprocess_create(command_line, 0, &subprocess);
        if (result) {
            // An error occurred! TODO: Throw an exception.
            assert(false);
        }
        childStdin = subprocess_stdin(&subprocess);
        childStdout = subprocess_stdout(&subprocess);
    }

    ~Subprocess() {
        // NOTE: This sends EOF to child process.
        fclose(childStdin);
        fclose(childStdout);
    }

    std::string readline() {
        // NOTE: Could write a fancier version of this that repeatedly `fgets` until reaching a newline.
        // (Dynamically growing a std::string as necessary to contain the full line.)
        // But this is probably good enough for our purposes.
        static const int max = 512;
        char line[max];
        fgets(line, max, childStdout);
        return line;
    }

    void write(const std::string &s) {
        fputs(s.c_str(), childStdin);
    }

    void writeline(const std::string &line) {
        write(line);
        fputc('\n', childStdin);
        fflush(childStdin);
    }

private:
    // Handle for child process
    struct subprocess_s subprocess;
    // Pipes to child process's childStdin (which we can write to) and childStdout (which we can read from)
    // (Renamed to avoid conflict with childStdin/childStdout macros on Windows.)
    FILE *childStdin, *childStdout;
};

class Stockfish {
public:
    Stockfish(const char *path="/usr/games/stockfish")
    : process(path) {
        assert(starts_with(process.readline(), "Stockfish"));
    }

    Chess::Move getMove(int time=1000) {
        process.write("go movetime ");
        process.writeline(std::to_string(time));
        std::string line = process.readline();
        while (!starts_with(line, "bestmove")) {
            line = process.readline();
        }
        auto words = split(line, std::regex(" "));
        return Chess::Move(words[1]);
    }

    void setState(const Chess::GameState &state) {
        process.write("position fen ");
        process.writeline(state.getFen());
    }

private:
    Subprocess process;
};

#endif
