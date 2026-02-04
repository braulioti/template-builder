#pragma once

#include <string>
#include <vector>
#include "types/PromptType.hpp"

namespace TemplateBuilder {

class CLINavigate {
public:
    CLINavigate() = default;
    ~CLINavigate() = default;

    static void runChecklistLoop(PromptInput* promptInput, std::vector<bool>& selected, size_t& currentIndex, bool& done);

private:
};

} // namespace TemplateBuilder
