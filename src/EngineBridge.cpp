#include <iostream>
#include <regex>
#include <vector>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "GameState.h"

// Concise way to split a string in C++ >= 11. (Based on https://stackoverflow.com/a/64886763/13204291.)
std::vector<std::string> split(const std::string &str, const std::regex &regex) {
    return std::vector<std::string>(std::sregex_token_iterator(str.begin(), str.end(), regex, -1), std::sregex_token_iterator());
}

class Subprocess {
public:
    Subprocess(const std::string &path) {
        pid_t pid = 0;
        int outpipe[2];
        int inpipe[2];
        pipe(outpipe);
        pipe(inpipe);
        pid = fork();
        if (pid == 0) {
            // We're in the child process.
            // Close unused pipe ends.
            close(inpipe[1]);
            close(outpipe[0]);
            // Redirect stdin/stdout/stderr to pipes.
            dup2(inpipe[0], STDIN_FILENO);
            // NOTE: Don't print any debug info after this!
            // It will just go into the pipe and get read by the parent process.
            dup2(outpipe[1], STDOUT_FILENO);
            dup2(outpipe[1], STDERR_FILENO);
            execl(path.c_str(), path.c_str(), (char*)NULL);
            // Executed only if `execl` failed:
            exit(1);
        }
        // We're in the parent process.
        // Close unused pipe ends.
        close(inpipe[0]);
        close(outpipe[1]);
        stdin = fdopen(inpipe[1], "w");
        stdout = fdopen(outpipe[0], "r");
    }

    ~Subprocess() {
        // NOTE: This sends EOF to child process.
        fclose(stdin);
        fclose(stdout);
    }

    std::string readline(int max=512) {
        // NOTE: Could write a fancier version of this that repeatedly `fgets` until reaching a newline.
        // (Dynamically growing a std::string as necessary to contain the full line.)
        // But this is probably good enough for our purposes.
        char line[max];
        fgets(line, max, stdout);
        return line;
    }

    void write(const std::string &s) {
        fputs(s.c_str(), stdin);
    }

    void writeline(const std::string &line) {
        write(line);
        fputc('\n', stdin);
        fflush(stdin);
    }

private:
    // PID of child process
    pid_t pid;
    // Pipes to child process's stdin (which we can write to) and stdout (which we can read from)
    FILE *stdin, *stdout;
};

class Stockfish {
public:
    Stockfish(const char *path="/usr/games/stockfish")
    : process(path) {
        assert(process.readline().starts_with("Stockfish"));
    }

    Chess::Move getMove(int time=1000) {
        process.write("go movetime ");
        process.writeline(std::to_string(time));
        std::string line = process.readline();
        while (!line.starts_with("bestmove")) {
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

// TODO: Put this in a test.
// int main() {
//     Stockfish engine;
//     std::cout << engine.getMove().toString() << std::endl;
//     std::cout << engine.getMove(100).toString() << std::endl;
//     return 0;
// }

using namespace Chess;

int main(int argc, const char **argv) {
    Stockfish engine;
    Game game(argc > 1 ? argv[1] : Game::initialFen);

    auto printBoard = [&game](){
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << "12345678"[rank] << "|";
            for (int file = 0; file < 8; file++) {
                std::optional<Piece> p = game.getPieceAt(Square(rank, file));
                std::cout << (p.has_value() ? p->toChar() : ' ') << "|";
            }
            std::cout << std::endl;
        }
        std::cout << "  ";
        for (int file = 0; file < 8; file++) {
            std::cout << (char)('a' + file) << " ";
        }
        std::cout << std::endl;
        std::cout << "FEN: " << game.getFen() << std::endl;
    };

    bool playing = true;
    while (playing) {
        printBoard();
        while (true) {
            std::string moveString;
            std::cout << "> ";
            std::cin >> moveString;
            if (moveString.empty()) {
                playing = false;
                break;
            }
            Move move(moveString);
            if (game.isLegal(move)) {
                game.push(move);
                break;
            } else {
                std::cout << "Illegal move!" << std::endl;
            }
        }
        printBoard();
        std::cout << "Engine is deciding on a move..." << std::endl;
        engine.setState(game);
        Move engineMove = engine.getMove();
        std::cout << "< " << engineMove.toString() << std::endl;
        assert(game.isLegal(engineMove));
        game.push(engineMove);
    }
}
