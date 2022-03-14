#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

    std::string readline(int max=512) {
        // NOTE: Could write a fancier version of this that repeatedly `fgets` until reaching a newline.
        // (Dynamically growing a std::string as necessary to contain the full line.)
        // But this is probably good enough for our purposes.
        char line[max];
        fgets(line, max, stdout);
        return line;
    }

    void writeline(const std::string &line) {
        fputs(line.c_str(), stdin);
        fputc('\n', stdin);
        fflush(stdin);
    }

private:
    FILE *stdin, *stdout;
};

class Stockfish {
public:
    Stockfish(const char *path="/usr/games/stockfish")
    : process(path) {
        assert(process.readline().starts_with("Stockfish"));
    }

    std::string get_best_move(int time=1000) {
        // TODO use `time`
        process.writeline("go movetime 1000");
        std::string line = process.readline();
        while (!line.starts_with("bestmove")) {
            line = process.readline();
        }
        return line; // TODO split
    }

private:
    Subprocess process;
};

int main() {
    Stockfish engine;
    std::cout << engine.get_best_move() << std::endl;
    std::cout << engine.get_best_move() << std::endl;
    return 0;
}
