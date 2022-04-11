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


    auto stream = juce::URL("https://lichess.org/api/stream/game/nbdMSf7h").createInputStream(options);
    REQUIRE(stream != nullptr);
    REQUIRE(statusCode == 200);
    std::cout << "Headers: " << responseHeaders.getDescription() << std::endl;
    std::cout << "Response body:" << std::endl;
    juce::String line;
    // juce::String prefix = "\"fen\":\"";
    do {
        line = stream->readNextLine();
        // auto segment = line.fromFirstOccurrenceOf(prefix, false, false).upToFirstOccurrenceOf("\"", false, false);
        juce::var v = juce::JSON::parse(line);
        if (v.hasProperty("id")) {
            std::cout << "Final FEN: " << std::endl;
        }
        std::cout << v.getProperty("fen", "").toString() << std::endl;
    } while (!line.isEmpty());
}
