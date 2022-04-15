#include <future>
#include <queue>
#include <string>

#include <juce_core/juce_core.h>

#include "GameState.h"

class GameStream {
public:
    GameStream(const std::string &gameID);
    bool finished();
    std::optional<Chess::Move> pollMove();

private:
    std::queue<Chess::Move> moves;
    std::mutex mutex;
    std::future<void> task;
    std::unique_ptr<juce::InputStream> stream;
};
