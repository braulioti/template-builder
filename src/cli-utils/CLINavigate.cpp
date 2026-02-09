#include "cli-utils/CLINavigate.hpp"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
// Undefine Windows macros that conflict with C++ standard library
#undef max
#undef min
#else
#include <termios.h>
#include <unistd.h>
#include <cstdio>
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

enum class ListKey { None, Up, Down, Enter, Escape };

#ifdef _WIN32
ListKey readListKey() {
    unsigned short key = 0;
    bool keyPressed = false;
    while (!readConsoleKey(key, keyPressed)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    if (key == VK_UP) return ListKey::Up;
    if (key == VK_DOWN) return ListKey::Down;
    if (key == VK_RETURN) return ListKey::Enter;
    if (key == VK_ESCAPE) return ListKey::Escape;
    return ListKey::None;
}
#else
ListKey readListKey() {
    if (!isatty(STDIN_FILENO)) {
        return ListKey::Enter;
    }
    struct termios saved;
    if (tcgetattr(STDIN_FILENO, &saved) != 0) {
        return ListKey::Enter;
    }
    struct termios raw = saved;
    raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) != 0) {
        return ListKey::Enter;
    }
    ListKey result = ListKey::None;
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\r' || c == '\n') {
            result = ListKey::Enter;
        } else if (c == 27) {
            raw.c_cc[VMIN] = 0;
            raw.c_cc[VTIME] = 1;
            tcsetattr(STDIN_FILENO, TCSANOW, &raw);
            unsigned char c2 = 0, c3 = 0;
            int n2 = read(STDIN_FILENO, &c2, 1);
            int n3 = (n2 == 1 && c2 == '[') ? read(STDIN_FILENO, &c3, 1) : 0;
            raw.c_cc[VMIN] = 1;
            raw.c_cc[VTIME] = 0;
            tcsetattr(STDIN_FILENO, TCSANOW, &raw);
            if (n2 == 1 && c2 == '[' && n3 == 1) {
                if (c3 == 'A') result = ListKey::Up;
                else if (c3 == 'B') result = ListKey::Down;
                else result = ListKey::Escape;
            } else {
                result = ListKey::Escape;
            }
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &saved);
    return result;
}
#endif

constexpr size_t LIST_WINDOW_SIZE = 6;

/** For display: show only the part after ';' if present, otherwise the whole string. */
static std::string displayPartOfItem(const std::string& item) {
    const size_t pos = item.find(';');
    return (pos != std::string::npos) ? item.substr(pos + 1) : item;
}

void renderListSelection(const std::vector<std::string>& items, size_t currentIndex) {
    const size_t n = items.size();
    const size_t windowSize = (n < LIST_WINDOW_SIZE) ? n : LIST_WINDOW_SIZE;
    const size_t startIndex = (currentIndex + 1 > windowSize) ? currentIndex - (windowSize - 1) : 0;

    for (size_t i = 0; i < windowSize; ++i) {
        const size_t itemIndex = startIndex + i;
        std::cout << (itemIndex == currentIndex ? "> " : "  ");
        std::cout << displayPartOfItem(items[itemIndex]) << std::endl;
    }
    std::cout << "Use Up/Down arrows to navigate, Enter to select" << std::endl;
}

} // namespace

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

std::optional<size_t> CLINavigate::selectFromList(const std::vector<std::string>& items) {
    if (items.empty()) {
        return std::nullopt;
    }
    size_t currentIndex = 0;
    const size_t windowSize = (items.size() < LIST_WINDOW_SIZE) ? items.size() : LIST_WINDOW_SIZE;
    const size_t lineCount = windowSize + 1;

    while (true) {
        renderListSelection(items, currentIndex);
        ListKey key = readListKey();
        clearDisplayedLines(lineCount);

        switch (key) {
            case ListKey::Up:
                if (currentIndex > 0) {
                    --currentIndex;
                }
                break;
            case ListKey::Down:
                if (currentIndex < items.size() - 1) {
                    ++currentIndex;
                }
                break;
            case ListKey::Enter:
                return currentIndex;
            case ListKey::Escape:
                return std::nullopt;
            case ListKey::None:
                break;
        }
    }
}

} // namespace TemplateBuilder
