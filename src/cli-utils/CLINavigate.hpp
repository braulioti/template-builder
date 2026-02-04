#pragma once

#include <string>
#include <vector>
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

private:
};

} // namespace TemplateBuilder
