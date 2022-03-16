#ifndef ENGINE_BRIDGE_H
#define ENGINE_BRIDGE_H

#include <iostream>
#include <regex>
#include <vector>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: Move this header to third-party folder.
#include "subprocess.h"

#include "GameState.h"

class Subprocess {
public:
    Subprocess(const std::string &path);
    ~Subprocess();
    std::string readline();
    void write(const std::string &s);
    void writeline(const std::string &line);

private:
    // Handle for child process
    struct subprocess_s subprocess;
    // Pipes to child process's childStdin (which we can write to) and childStdout (which we can read from)
    // (Renamed to avoid conflict with childStdin/childStdout macros on Windows.)
    FILE *childStdin, *childStdout;
};

class Stockfish {
public:
    Stockfish(const std::string &path);
    Chess::Move getMove(int time=1000);
    void setState(const Chess::GameState &state);

private:
    Subprocess process;
};

#endif
