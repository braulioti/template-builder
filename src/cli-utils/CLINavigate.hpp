#pragma once

#include <string>
#include <vector>
#include <optional>
#include "types/PromptType.hpp"

namespace TemplateBuilder {

struct ChecklistLoopParams {
    PromptInput* promptInput;
    std::vector<bool>& selected;
    size_t& currentIndex;
    bool& done;

    ChecklistLoopParams(PromptInput* pi, std::vector<bool>& sel, size_t& idx, bool& d)
        : promptInput(pi), selected(sel), currentIndex(idx), done(d) {}
};

class CLINavigate {
public:
    CLINavigate() = default;
    ~CLINavigate() = default;

    static void runChecklistLoop(ChecklistLoopParams& params);

    /**
     * Displays a list of items and lets the user choose one with Up/Down arrows.
     * Enter confirms the selection. Returns the selected index, or std::nullopt if
     * the list is empty or selection was cancelled (Escape).
     */
    static std::optional<size_t> selectFromList(const std::vector<std::string>& items);

private:
};

} // namespace TemplateBuilder
