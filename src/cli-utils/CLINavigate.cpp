#include "cli-utils/CLINavigate.hpp"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
// Undefine Windows macros that conflict with C++ standard library
#undef max
#undef min
#endif

namespace TemplateBuilder {

#ifdef _WIN32
namespace {

bool readConsoleKey(unsigned short& key, bool& keyPressed) {
    keyPressed = false;
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        return false;
    }

    INPUT_RECORD inputRecord;
    DWORD numRead;

    if (!PeekConsoleInput(hStdin, &inputRecord, 1, &numRead)) {
        return false;
    }

    if (numRead == 0) {
        return false;
    }

    if (ReadConsoleInput(hStdin, &inputRecord, 1, &numRead)) {
        if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
            key = inputRecord.Event.KeyEvent.wVirtualKeyCode;
            keyPressed = true;
            return true;
        }
    }

    return false;
}

} // namespace
#else
namespace {

bool readConsoleKey(unsigned short& key, bool& keyPressed) {
    keyPressed = false;
    return false;
}

} // namespace
#endif

void CLINavigate::runChecklistLoop(PromptInput* promptInput, std::vector<bool>& selected, size_t& currentIndex, bool& done) {
    const auto& options = promptInput->getOptions();
    while (!done) {
        for (size_t i = 0; i < options.size(); ++i) {
            std::cout << (i == currentIndex ? "> " : "  ");
            std::cout << (selected[i] ? "[ X ] " : "[   ] ");
            std::cout << options[i]->getName() << std::endl;
        }
        std::cout << "Use Up/Down arrows to navigate, Space to select/deselect, Enter to confirm" << std::endl;
#ifdef _WIN32
        while (true) {
            unsigned short key = 0;
            bool keyPressed = false;
            if (readConsoleKey(key, keyPressed)) {
                for (size_t i = 0; i < options.size() + 1; ++i) {
                    std::cout << "\033[A\033[2K";
                }
                if (key == VK_UP && currentIndex > 0) {
                    --currentIndex;
                } else if (key == VK_DOWN && currentIndex < options.size() - 1) {
                    ++currentIndex;
                } else if (key == VK_SPACE) {
                    selected[currentIndex] = !selected[currentIndex];
                } else if (key == VK_RETURN) {
                    done = true;
                }
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
#else
        done = true;
#endif
    }
}

} // namespace TemplateBuilder
