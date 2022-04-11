#include <iostream>
#include <juce_core/juce_core.h>
#include "catch.hpp"

TEST_CASE("fetch a game from lichess", "[fetch]") {
    juce::StringPairArray responseHeaders;
    int statusCode = 0;

    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
        .withConnectionTimeoutMs(-1)
        .withResponseHeaders(&responseHeaders)
        .withStatusCode(&statusCode);


    auto stream = juce::URL("https://lichess.org/api/stream/game/E2tw43AQ").createInputStream(options);
    REQUIRE(stream != nullptr);
    REQUIRE(statusCode == 200);
    std::cout << "Headers: " << responseHeaders.getDescription() << std::endl;
    std::cout << "Response body:" << std::endl;
    juce::String line;
    while ((line = stream->readNextLine()).isNotEmpty()) {
        if (line.isEmpty()) break;
        std::cout << "Raw: " << line << std::endl;
        juce::var obj = juce::JSON::parse(line);
        if (obj.hasProperty("id")) {
            // Can check `obj.status` to determine if game is ongoing.
            std::cout << "Latest FEN: ";
        }
        if (obj.hasProperty("wc")) {
            int wc = obj.getProperty("wc", -1);
            int bc = obj.getProperty("bc", -1);
            std::cout << "Clock info: " << wc << "|" << bc << " ";
        }
        std::cout << "FEN: " << obj.getProperty("fen", "").toString() << std::endl;
    }
}
