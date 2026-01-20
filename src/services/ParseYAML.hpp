#pragma once

#include <string>
#include <vector>
#include <memory>
#include <yaml-cpp/yaml.h>
#include "types/VariableType.hpp"
#include "types/PromptType.hpp"
#include "types/FileType.hpp"
#include "types/RemoteFileType.hpp"
#include "builders/FileBuilder.hpp"
#include "builders/FolderBuilder.hpp"
#include "builders/RemoteFileBuilder.hpp"

namespace TemplateBuilder {

class UnsupportedTemplateVersion : public std::exception {
public:
    explicit UnsupportedTemplateVersion(const std::string& message) : m_message(message) {}
    [[nodiscard]] const char* what() const noexcept override { return m_message.c_str(); }
private:
    std::string m_message;
};

class ParserYAML {
public:
    explicit ParserYAML(const std::string& fileName);
    ~ParserYAML() = default;

    // Main method to build all files, folders, and remote files
    void buildAll();

    // Getters
    [[nodiscard]] const std::string& getVersion() const noexcept { return m_version; }
    [[nodiscard]] const std::vector<std::unique_ptr<Variable>>& getVariables() const noexcept { return m_variables; }
    [[nodiscard]] const std::vector<std::unique_ptr<Prompt>>& getPrompts() const noexcept { return m_prompts; }
    [[nodiscard]] const std::vector<std::unique_ptr<FileData>>& getFiles() const noexcept { return m_files; }
    [[nodiscard]] const std::vector<std::unique_ptr<FileData>>& getFolders() const noexcept { return m_folders; }
    [[nodiscard]] const std::vector<std::unique_ptr<RemoteFileData>>& getRemoteFiles() const noexcept { return m_remoteFiles; }

private:
    // Initialization
    void initialize();

    // Loading methods
    void loadVariables(const YAML::Node& root);
    void loadPrompts(const YAML::Node& root);
    void loadFiles(const YAML::Node& root);
    void loadFolders(const YAML::Node& root);
    void loadRemoteFiles(const YAML::Node& root);

    // Validation
    void validateVersion(const std::string& version) const;

    // Helper methods
    [[nodiscard]] Variable* findVariableByName(const std::string& name) const;
    [[nodiscard]] Prompt* findPromptByName(const std::string& name) const;
    [[nodiscard]] static VariableType parseVariableType(const std::string& typeStr);
    [[nodiscard]] static PromptType parsePromptType(const std::string& typeStr);
    [[nodiscard]] static std::string toLower(const std::string& str);

    std::string m_version;
    YAML::Node m_doc;
    std::vector<std::unique_ptr<Variable>> m_variables;
    std::vector<Variable*> m_variablesPtrs;  // Non-owning pointers for FileData
    std::vector<std::unique_ptr<Prompt>> m_prompts;
    std::vector<std::unique_ptr<FileData>> m_files;
    std::vector<std::unique_ptr<FileData>> m_folders;
    std::vector<std::unique_ptr<RemoteFileData>> m_remoteFiles;
    std::unique_ptr<FileBuilder> m_fileBuilder;
    std::unique_ptr<FolderBuilder> m_folderBuilder;
    std::unique_ptr<RemoteFileBuilder> m_remoteFileBuilder;
};

} // namespace TemplateBuilder
