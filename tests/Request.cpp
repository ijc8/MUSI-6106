#include <iostream>
#include <juce_core/juce_core.h>
#include "catch.hpp"

TEST_CASE("make an HTTP request", "[board]") {
    juce::StringPairArray responseHeaders;
    int statusCode = 0;

    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
        .withConnectionTimeoutMs(10000)
        .withResponseHeaders(&responseHeaders)
        .withStatusCode(&statusCode);


    auto stream = juce::URL("https://lichess.org/api/stream/game/1lkY0VZk").createInputStream(options);
    REQUIRE(stream != nullptr);
    REQUIRE(statusCode == 200);
    std::cout << "Headers: " << responseHeaders.getDescription() << std::endl;
    std::cout << "Response body:" << std::endl;
    std::cout << stream->readEntireStreamAsString();
}
