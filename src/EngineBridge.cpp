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

Analysis Stockfish::analyze(const Chess::GameState &state, int time) {
    // Set up the position.
    process.write("position fen ");
    process.writeline(state.getFen());
    // Start computing.
    process.write("go movetime ");
    process.writeline(std::to_string(time));
    // Read output: we're looking for `bestmove ...`,
    // preceded by a final `info ...` line with the score.
    std::string infoLine = process.readline();
    std::string moveLine = process.readline();
    while (!starts_with(moveLine, "bestmove")) {
        infoLine = moveLine;
        moveLine = process.readline();
    }
    // Extract relevant info: best move, overall evaluation score.
    auto infoWords = split(infoLine, std::regex(" "));
    auto moveWords = split(moveLine, std::regex(" "));
    return { Chess::Move(moveWords[1]), std::stod(infoWords[9]) };
}
