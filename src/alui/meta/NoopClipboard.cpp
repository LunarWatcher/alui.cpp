#include "Clipboard.hpp"

namespace alui {

bool Clipboard::hasClipboardImpl = false;

std::string Clipboard::getContent() {
    return "";
}

extern void Clipboard::setContent(const std::string&) {}

}
