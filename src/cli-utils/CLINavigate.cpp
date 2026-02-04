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

namespace {

void renderChecklist(const ChecklistLoopParams& params) {
    const auto& options = params.promptInput->getOptions();
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << (i == params.currentIndex ? "> " : "  ");
        std::cout << (params.selected[i] ? "[ X ] " : "[   ] ");
        std::cout << options[i]->getName() << std::endl;
    }
    std::cout << "Use Up/Down arrows to navigate, Space to select/deselect, Enter to confirm" << std::endl;
}

void clearDisplayedLines(size_t lineCount) {
    for (size_t i = 0; i < lineCount; ++i) {
        std::cout << "\033[A\033[2K";
    }
}

#ifdef _WIN32
void handleKeyPress(ChecklistLoopParams& params, unsigned short key) {
    const auto& options = params.promptInput->getOptions();
    if (key == VK_UP && params.currentIndex > 0) {
        --params.currentIndex;
    } else if (key == VK_DOWN && params.currentIndex < options.size() - 1) {
        ++params.currentIndex;
    } else if (key == VK_SPACE) {
        params.selected[params.currentIndex] = !params.selected[params.currentIndex];
    } else if (key == VK_RETURN) {
        params.done = true;
    }
}

bool processSingleKeyInput(ChecklistLoopParams& params) {
    unsigned short key = 0;
    bool keyPressed = false;
    if (!readConsoleKey(key, keyPressed)) {
        return false;
    }
    const auto& options = params.promptInput->getOptions();
    clearDisplayedLines(options.size() + 1);
    handleKeyPress(params, key);
    return true;
}

void waitForAndProcessKey(ChecklistLoopParams& params) {
    while (!processSingleKeyInput(params)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
#endif

} // namespace

void CLINavigate::runChecklistLoop(ChecklistLoopParams& params) {
    while (!params.done) {
        renderChecklist(params);
#ifdef _WIN32
        waitForAndProcessKey(params);
#else
        params.done = true;
#endif
    }
}

} // namespace TemplateBuilder
