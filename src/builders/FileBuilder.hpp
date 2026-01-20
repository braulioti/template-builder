#pragma once

#include <string>
#include <memory>
#include "types/FileType.hpp"
#include "builders/PromptBuilder.hpp"

namespace TemplateBuilder {

class FileBuilder {
public:
    FileBuilder();
    ~FileBuilder() = default;

    // Main method to build a file
    void build(const FileData* file);

private:
    std::unique_ptr<PromptBuilder> m_promptBuilder;
};

} // namespace TemplateBuilder
