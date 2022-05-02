#include "GameStream.h"

GameStream::GameStream(const std::string &gameID, std::function<void (std::optional<Chess::Move>)> onUpdate) {
    // Connect immediately so that we can throw an exception if needed.
    juce::StringPairArray responseHeaders;
    int statusCode = 0;

    // NOTE: We use a JUCE thread here so we can forcefully terminate it.
    // (We might have to forcefully terminate it because JUCE does not provide
    // a way to set a timeout for a single read, and WebInputStream::cancel
    // does not actually cancel blocking reads, at least with the Curl backend.)
    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
        .withConnectionTimeoutMs(-1)
        .withResponseHeaders(&responseHeaders)
        .withStatusCode(&statusCode);

    const std::string baseURL = "https://lichess.org/api/stream/game/";
    stream.reset(dynamic_cast<juce::WebInputStream *>(juce::URL(baseURL + gameID).createInputStream(options).release()));
    if (stream == nullptr || statusCode != 200) {
        throw std::runtime_error("Lichess connection failed.");
    }

    thread = std::make_unique<StreamThread>(*stream.get(), onUpdate);
    thread->startThread();
}

GameStream::~GameStream() {
    cancel();
}

void GameStream::cancel() {
    if (stream) stream->cancel();
    if (thread) thread->stopThread(100);
    stream.reset();
    thread.reset();
}

bool GameStream::finished() {
    return !thread->isThreadRunning();
}
