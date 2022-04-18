#include "GameStream.h"

GameStream::GameStream(const std::string &gameID) {
    // Connect immediately so that we can throw an exception if needed.
    juce::StringPairArray responseHeaders;
    int statusCode = 0;

    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
        .withConnectionTimeoutMs(-1)
        .withResponseHeaders(&responseHeaders)
        .withStatusCode(&statusCode);

    const std::string baseURL = "https://lichess.org/api/stream/game/";
    stream = juce::URL(baseURL + gameID).createInputStream(options);
    if (stream == nullptr || statusCode != 200) {
        throw std::runtime_error("Lichess connection failed.");
    }

    running = true;

    // Then, pull moves from stream in separate thread.
    task = std::async(std::launch::async, [this] {
        juce::String line;
        while (running && (line = stream->readNextLine()).isNotEmpty()) {
            if (line.isEmpty()) break;
            juce::var obj = juce::JSON::parse(line);
            if (obj.hasProperty("lm")) {
                // Enqueue the last move.
                Chess::Move move(obj.getProperty("lm", "").toString().toStdString());
                std::unique_lock<std::mutex> lock(mutex);
                moves.push(move);
            }
            // Additional properties we might be interested in: fen, wc/bc (clock time), status.
            // NOTE: We ignore the possibility of resignation for now.
        }
    });
}

GameStream::~GameStream() {
    cancel();
}

void GameStream::cancel() {
    running = false;
}

bool GameStream::finished() {
    // If the task is finished and there are no queued moves, we're done.
    std::unique_lock<std::mutex> lock(mutex);
    return moves.empty() && !task.valid();
}

std::optional<Chess::Move> GameStream::pollMove() {
    // Check if any new moves are available. If so, return the next one in the queue.
    std::unique_lock<std::mutex> lock(mutex);
    if (moves.empty()) {
        return std::nullopt;
    }
    Chess::Move move = moves.front();
    moves.pop();
    return move;
}
