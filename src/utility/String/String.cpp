#include "String.hpp"

namespace String {

std::string repeat(const std::string& str, unsigned int times) {
    std::string result = "";

    for (unsigned int i = 0; i < times; i++) {
        result += str;
    }

    return result;
}

}