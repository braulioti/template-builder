#include "cli-utils/CLINavigate.hpp"
#include "cli-utils/CLINavigateKeys.hpp"
#include <iostream>
#include <optional>

#ifdef _WIN32
#include <windows.h>
#undef max
#undef min
#endif
#include <thread>
#include <chrono>

namespace TemplateBuilder {

namespace {

constexpr size_t LIST_WINDOW_SIZE = 6;

struct ListKeyOutcome {
    size_t nextIndex;
    bool done;
    std::optional<size_t> returnValue;
};

ListKeyOutcome applyListKey(CLINavigateKeys::Key key, size_t currentIndex, size_t itemCount) {
    ListKeyOutcome out{currentIndex, false, std::nullopt};
    switch (key) {
        case CLINavigateKeys::Key::Up:
            if (currentIndex > 0) out.nextIndex = currentIndex - 1;
            break;
        case CLINavigateKeys::Key::Down:
            if (currentIndex < itemCount - 1) out.nextIndex = currentIndex + 1;
            break;
        case CLINavigateKeys::Key::Enter:
            out.done = true;
            out.returnValue = currentIndex;
            break;
        case CLINavigateKeys::Key::Escape:
            out.done = true;
            out.returnValue = std::nullopt;
            break;
        default:
            break;
    }
    return out;
}

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

static void handleChecklistKey(ChecklistLoopParams& params, CLINavigateKeys::Key key) {
    const auto& options = params.promptInput->getOptions();
    switch (key) {
        case CLINavigateKeys::Key::Up:
            if (params.currentIndex > 0) --params.currentIndex;
            break;
        case CLINavigateKeys::Key::Down:
            if (params.currentIndex < options.size() - 1) ++params.currentIndex;
            break;
        case CLINavigateKeys::Key::Space:
            params.selected[params.currentIndex] = !params.selected[params.currentIndex];
            break;
        case CLINavigateKeys::Key::Enter:
            params.done = true;
            break;
        default:
            break;
    }
}

#ifdef _WIN32
static void waitForAndProcessChecklistKey(ChecklistLoopParams& params) {
    const auto& options = params.promptInput->getOptions();
    clearDisplayedLines(options.size() + 1);
    handleChecklistKey(params, CLINavigateKeys::read());
}
#endif

} // namespace

void CLINavigate::runChecklistLoop(ChecklistLoopParams& params) {
    while (!params.done) {
        renderChecklist(params);
#ifdef _WIN32
        waitForAndProcessChecklistKey(params);
#else
        params.done = true;
#endif
    }
}

std::optional<size_t> CLINavigate::selectFromList(const std::vector<std::string>& items) {
    if (items.empty()) return std::nullopt;
    size_t currentIndex = 0;
    const size_t windowSize = (items.size() < LIST_WINDOW_SIZE) ? items.size() : LIST_WINDOW_SIZE;
    const size_t lineCount = windowSize + 1;

    while (true) {
        renderListSelection(items, currentIndex);
        const CLINavigateKeys::Key key = CLINavigateKeys::read();
        clearDisplayedLines(lineCount);
        ListKeyOutcome outcome = applyListKey(key, currentIndex, items.size());
        if (outcome.done) return outcome.returnValue;
        currentIndex = outcome.nextIndex;
    }
}

} // namespace TemplateBuilder
