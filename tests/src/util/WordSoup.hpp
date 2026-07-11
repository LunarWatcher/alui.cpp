#pragma once

#include <cstddef>
#include <random>
#include <string>
#include <sstream>
#include <vector>

namespace test {

namespace _detail {

// contains 100 random words used for generating a word soup. These were actually picked
// at random with a random word picker, and then partly filtered to avoid accidentally being able to make sentences
const inline std::vector<std::string> words = {
    "ocean", "thunder", "velvet", "lantern", "crisp", "meadow", "sapphire", "whisper", "jungle", "pebble",
    "horizon", "ember", "cascade", "riddle", "breeze", "willow", "fragment", "quartz", "drift", "summit", "harbor",
    "twilight", "marble", "echo", "feather", "canyon", "spark", "orchard", "ripple", "cobalt", "fungus", "glacier",
    "lantern", "tempest", "rivulet", "meadow", "opal", "shudder", "cliffside", "nectar", "compass", "garnet",
    "bramble", "zephyr", "tundra", "ivory", "fern", "granite", "solstice", "mirage",
    "prairie", "blizzard", "saffron", "falcon", "obelisk", "marigold", "cinder", "temple", "vortex", "jasmine",
    "lavender", "quicksilver", "glade", "lighthouse", "basil", "monsoon", "pearl", "dune", "cedar", "firefly", "grotto",
    "mahogany", "cyclone", "azalea", "thicket", "obsidian", "halcyon", "riverbed", "coriander", "aurora", "stalactite",
    "maple", "gale", "silk", "amethyst", "ravine", "clover", "pyramid", "indigo", "boulder", "wisteria", "tidepool",
    "moss", "granary", "phoenix", "sorrel", "stalwart", "midnight", "frost", "canopy"
};

}

/**
 * Generates a word soup, a string with words structured into something that passably looks structured as text. This is
 * basically "we have lorem ipsum at home" without any of lorem ipsum being used. This is largely because doing lorem
 * ipsum properly may or may not require a markov chain, and that's wayyyyyyyyyyyy too much effort when all I need is a
 * word soup.
 */
inline std::string getWordSoup(size_t words = 50, size_t seed = 69) {
    std::mt19937 rand(seed);
    std::uniform_int_distribution<size_t> wordDist(0, _detail::words.size() - 1);
    // Used to generate the next structure part (a comma, period, space, or newline).
    // This, and all numbers associated with it are magic numbers that were found experimentally with the default seed.
    // With the default seed, this generates a passable 3 paragraphs with vaguely nice structure (sampled at N = 150)
    // There's no special reason for these values that can be attributed to anything other than it generating
    // sentence-shaped things that aren't actively horrible.
    std::normal_distribution<double> structureDist(5, 1.45);

    std::stringstream ss;
    for (size_t i = 0; i < words; ++i) {
        ss << _detail::words.at(wordDist(rand));

        size_t punct = std::lround(structureDist(rand));
        // ss << punct << std::endl;
        if (punct >= 4 && punct <= 6) {
            if (i != words - 1) {
                ss << " ";
            }
        } else if (
            punct == 3 || punct == 7
            // Avoid trailing commas
            || ((punct == 2 || punct == 8) && i == words - 1)
        ) {
            ss << ".";
            if (i != words - 1) {
                ss << " ";
            }
        } else if (punct == 2 || punct == 8) {
            ss << ",";
            if (i != words - 1) {
                ss << " ";
            }
        } else {
            ss << ".";
            if (i != words - 1) {
                ss << "\n\n";
            }
            continue;
        }
    }

    return ss.str();
}

}
