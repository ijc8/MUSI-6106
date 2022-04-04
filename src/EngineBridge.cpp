#include "EngineBridge.h"

// Concise way to split a string in C++ >= 11. (Based on https://stackoverflow.com/a/64886763/13204291.)
std::vector<std::string> split(const std::string &str, const std::regex &regex) {
    return std::vector<std::string>(std::sregex_token_iterator(str.begin(), str.end(), regex, -1), std::sregex_token_iterator());
}

// TODO: Consider using C++20 (we're using C++17 now) to get this for free.
bool starts_with(const std::string &str, const std::string &prefix) {
    return str.rfind(prefix, 0) == 0;
}


Subprocess::Subprocess(const std::string &path) {
    const char *command_line[] = {path.c_str(), NULL};
    int result = subprocess_create(command_line, 0, &subprocess);
    if (result) {
        // An error occurred! TODO: Throw an exception.
        assert(false);
    }
    childStdin = subprocess_stdin(&subprocess);
    childStdout = subprocess_stdout(&subprocess);
}

Subprocess::~Subprocess() {
    // NOTE: This sends EOF to child process.
    fclose(childStdin);
    fclose(childStdout);
}

std::string Subprocess::readline() {
    // NOTE: Could write a fancier version of this that repeatedly `fgets` until reaching a newline.
    // (Dynamically growing a std::string as necessary to contain the full line.)
    // But this is probably good enough for our purposes.
    static const int max = 512;
    char line[max];
    fgets(line, max, childStdout);
    return line;
}

void Subprocess::write(const std::string &s) {
    fputs(s.c_str(), childStdin);
}

void Subprocess::writeline(const std::string &line) {
    write(line);
    fputc('\n', childStdin);
    fflush(childStdin);
}

Stockfish::Stockfish(const std::string &path) : process(path) {
    assert(starts_with(process.readline(), "Stockfish"));
}

Chess::Move Stockfish::getMove(int time) {
    process.write("go movetime ");
    process.writeline(std::to_string(time));
    std::string line = process.readline();
    while (!starts_with(line, "bestmove")) {
        line = process.readline();
    }
    auto words = split(line, std::regex(" "));
    return Chess::Move(words[1]);
}

void Stockfish::setState(const Chess::GameState &state) {
    process.write("position fen ");
    process.writeline(state.getFen());
}

Analysis Stockfish::analyze(const Chess::GameState &state, int time) {
    setState(state);
    process.write("go movetime ");
    process.writeline(std::to_string(time));
    std::string prevLine = process.readline();
    std::string line = process.readline();
    while (!starts_with(line, "bestmove")) {
        prevLine = line;
        line = process.readline();
    }
    auto prevWords = split(prevLine, std::regex(" "));
    auto words = split(line, std::regex(" "));
    return { Chess::Move(words[1]), std::stod(prevWords[9]) };
}
