#include <future>
#include <string>

#include <juce_core/juce_core.h>

#include "GameState.h"

class GameStream {
public:
    GameStream(const std::string &gameID, std::function<void (std::optional<Chess::Move>)> onUpdate);
    ~GameStream();
    void cancel();
    bool finished();
    std::optional<Chess::Move> pollMove();

private:
    class StreamThread: public juce::Thread {
    public:
        StreamThread(juce::WebInputStream &stream, std::function<void (std::optional<Chess::Move>)> onUpdate)
        : stream(stream), onUpdate(onUpdate), Thread("Stream thread") {}

        void run() override {
            juce::String line;
            while (!(threadShouldExit() || (line = stream.readNextLine()).isEmpty())) {
                juce::var obj = juce::JSON::parse(line);
                if (obj.hasProperty("lm")) {
                    Chess::Move move(obj.getProperty("lm", "").toString().toStdString());
                    onUpdate(move);
                }
                // Additional properties we might be interested in: fen, wc/bc (clock time), status.
                // NOTE: We ignore the possibility of resignation for now.
            }
            onUpdate(std::nullopt);
        }
    
    private:
        juce::WebInputStream &stream;
        std::function<void (std::optional<Chess::Move>)> onUpdate;
    };

    std::unique_ptr<juce::WebInputStream> stream;
    std::unique_ptr<StreamThread> thread;
};
