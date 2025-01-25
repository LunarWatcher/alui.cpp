#pragma once

#include <string>

namespace alui::Clipboard {

extern bool hasClipboardImpl;

extern std::string getContent();
extern void setContent(const std::string& str);

}
