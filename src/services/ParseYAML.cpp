#include "services/ParseYAML.hpp"
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <filesystem>

namespace TemplateBuilder {

namespace {
    // Keep older templates working while supporting the current spec.
    constexpr const char* SUPPORTED_VERSIONS[] = {"0.1", "1.0"};
    constexpr size_t SUPPORTED_VERSIONS_COUNT = sizeof(SUPPORTED_VERSIONS) / sizeof(SUPPORTED_VERSIONS[0]);
}

ParserYAML::ParserYAML(const std::string& fileName) {
    if (fileName.empty()) {
        throw std::invalid_argument("YAML file not provided.");
    }

    if (!std::filesystem::exists(fileName)) {
        throw std::runtime_error("YAML file not found: " + fileName);
    }

    // Load YAML
    try {
        m_doc = YAML::LoadFile(fileName);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Failed to parse YAML file: " + std::string(e.what()));
    }

    // Access 'version' key
    if (!m_doc["version"]) {
        throw std::runtime_error("Required field \"version\" not found in YAML.");
    }

    m_version = m_doc["version"].as<std::string>();

    validateVersion(m_version);
    initialize();
    
    const YAML::Node& root = m_doc;
    loadVariables(root);
    loadPrompts(root);
    loadFiles(root);
    loadFolders(root);
    loadRemoteFiles(root);
}

void ParserYAML::initialize() {
    m_fileBuilder = std::make_unique<FileBuilder>();
    m_folderBuilder = std::make_unique<FolderBuilder>();
    m_remoteFileBuilder = std::make_unique<RemoteFileBuilder>();
}

void ParserYAML::validateVersion(const std::string& version) const {
    bool found = false;
    for (size_t i = 0; i < SUPPORTED_VERSIONS_COUNT; ++i) {
        if (toLower(version) == toLower(SUPPORTED_VERSIONS[i])) {
            found = true;
            break;
        }
    }
    
    if (!found) {
        std::string supportedVersionsStr;
        bool first = true;
        for (size_t i = 0; i < SUPPORTED_VERSIONS_COUNT; ++i) {
            if (!first) supportedVersionsStr += ", ";
            supportedVersionsStr += SUPPORTED_VERSIONS[i];
            first = false;
        }
        throw UnsupportedTemplateVersion(
            "Template version not supported: " + version + ". Supported versions: " + supportedVersionsStr
        );
    }
}

void ParserYAML::loadVariables(const YAML::Node& root) {
    if (!root["variables"]) {
        return; // No variables section, list remains empty
    }

    const YAML::Node& variablesNode = root["variables"];
    if (!variablesNode.IsSequence()) {
        throw std::runtime_error("\"variables\" must be a sequence (array) in YAML.");
    }

    m_variables.clear();
    m_variablesPtrs.clear();
    for (size_t i = 0; i < variablesNode.size(); ++i) {
        const YAML::Node& variableItem = variablesNode[i];

        auto variableObj = std::make_unique<Variable>();
        variableObj->setName(variableItem["name"].as<std::string>());

        std::string typeStr = variableItem["type"].as<std::string>();
        variableObj->setType(parseVariableType(typeStr));

        // Optional value field
        if (variableItem["value"]) {
            variableObj->setValue(variableItem["value"].as<std::string>());
        }

        m_variablesPtrs.push_back(variableObj.get());
        m_variables.push_back(std::move(variableObj));
    }
}

void ParserYAML::loadPromptInput(Prompt* promptObj, const YAML::Node& inputItem, size_t inputIndex, size_t promptIndex) {
    auto promptInput = std::make_unique<PromptInput>();
    promptInput->setInput(inputItem["input"].Scalar());
    std::string variableName = inputItem["variable"].as<std::string>();
    Variable* foundVariable = findVariableByName(variableName);
    if (foundVariable == nullptr) {
        throw std::runtime_error(
            "Variable \"" + variableName + "\" not found for input at index " +
            std::to_string(inputIndex) + " in prompt at index " + std::to_string(promptIndex) + "."
        );
    }
    promptInput->setVariable(foundVariable);
    promptInput->setType(parsePromptType(inputItem["type"].as<std::string>()));
    if (inputItem["options"]) {
        const YAML::Node& optionsNode = inputItem["options"];
        if (!optionsNode.IsSequence()) {
            throw std::runtime_error(
                "\"options\" must be a sequence (array) for input at index " +
                std::to_string(inputIndex) + " in prompt at index " + std::to_string(promptIndex) + "."
            );
        }
        for (size_t k = 0; k < optionsNode.size(); ++k) {
            const YAML::Node& optionItem = optionsNode[k];
            auto optionObj = std::make_unique<PromptInputOption>(
                optionItem["name"].as<std::string>(),
                optionItem["value"].as<std::string>()
            );
            promptInput->addOption(std::move(optionObj));
        }
    }
    promptObj->addInput(std::move(promptInput));
}

void ParserYAML::loadSinglePrompt(const YAML::Node& promptItem, size_t promptIndex) {
    auto promptObj = std::make_unique<Prompt>();
    promptObj->setName(promptItem["name"].as<std::string>());
    if (promptItem["result"]) {
        promptObj->setResult(promptItem["result"].as<std::string>());
    }
    if (promptItem["inputs"]) {
        const YAML::Node& inputsNode = promptItem["inputs"];
        if (!inputsNode.IsSequence()) {
            throw std::runtime_error("\"inputs\" must be a sequence (array) for prompt at index " + std::to_string(promptIndex) + ".");
        }
        for (size_t j = 0; j < inputsNode.size(); ++j) {
            loadPromptInput(promptObj.get(), inputsNode[j], j, promptIndex);
        }
    }
    m_prompts.push_back(std::move(promptObj));
}

void ParserYAML::loadPrompts(const YAML::Node& root) {
    if (!root["prompts"]) {
        return;
    }
    const YAML::Node& promptsNode = root["prompts"];
    if (!promptsNode.IsSequence()) {
        throw std::runtime_error("\"prompts\" must be a sequence (array) in YAML.");
    }
    m_prompts.clear();
    for (size_t i = 0; i < promptsNode.size(); ++i) {
        loadSinglePrompt(promptsNode[i], i);
    }
}

void ParserYAML::loadFiles(const YAML::Node& root) {
    if (!root["files"] || !root["files"].IsSequence()) {
        return; // No files section, list remains empty
    }

    const YAML::Node& filesNode = root["files"];
    m_files.clear();
    
    for (size_t i = 0; i < filesNode.size(); ++i) {
        const YAML::Node& fileItem = filesNode[i];

        // Content is optional - if not specified, file will be created empty
        std::string content;
        if (fileItem["content"]) {
            content = fileItem["content"].as<std::string>();
        }

        auto fileObj = std::make_unique<FileData>(
            fileItem["path"].as<std::string>(),
            content
        );

        // Assign variables reference to file
        fileObj->setVariables(&m_variablesPtrs);

        // Prompt is optional - if not specified and content is also empty, file will be created empty
        if (fileItem["prompt"]) {
            std::string promptName = fileItem["prompt"].as<std::string>();
            Prompt* foundPrompt = findPromptByName(promptName);
            fileObj->setPrompt(foundPrompt);
        }

        m_files.push_back(std::move(fileObj));
    }
}

void ParserYAML::loadFolders(const YAML::Node& root) {
    if (!root["folders"] || !root["folders"].IsSequence()) {
        return; // No folders section, list remains empty
    }

    const YAML::Node& foldersNode = root["folders"];
    m_folders.clear();
    
    for (size_t i = 0; i < foldersNode.size(); ++i) {
        const YAML::Node& folderItem = foldersNode[i];

        // Folders only need path, content is empty
        auto folderObj = std::make_unique<FileData>(
            folderItem["path"].as<std::string>(),
            ""
        );

        m_folders.push_back(std::move(folderObj));
    }
}

void ParserYAML::loadRemoteFiles(const YAML::Node& root) {
    if (!root["remote"] || !root["remote"].IsSequence()) {
        return; // No remote section, list remains empty
    }

    const YAML::Node& remoteNode = root["remote"];
    m_remoteFiles.clear();
    
    for (size_t i = 0; i < remoteNode.size(); ++i) {
        const YAML::Node& remoteItem = remoteNode[i];

        auto remoteObj = std::make_unique<RemoteFileData>(
            remoteItem["uri"].as<std::string>(),
            remoteItem["path"].as<std::string>()
        );

        m_remoteFiles.push_back(std::move(remoteObj));
    }
}

void ParserYAML::buildFolders() {
    for (const auto& folderItem : m_folders) {
        try {
            m_folderBuilder->build(folderItem.get());
            std::cout << "Created folder " << folderItem->getPath() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error creating folder " << folderItem->getPath() << ": " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error creating folder " << folderItem->getPath() << std::endl;
        }
    }
}

void ParserYAML::buildFiles() {
    for (const auto& fileItem : m_files) {
        try {
            m_fileBuilder->build(fileItem.get());
            std::cout << "Created file " << fileItem->getPath() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error creating file " << fileItem->getPath() << ": " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error creating file " << fileItem->getPath() << std::endl;
        }
        std::cout.flush();
        std::cerr.flush();
    }
}

void ParserYAML::buildRemoteFiles() {
    for (const auto& remoteFileItem : m_remoteFiles) {
        try {
            m_remoteFileBuilder->build(remoteFileItem.get());
        } catch (const std::exception& e) {
            std::cerr << "Error downloading remote file " << remoteFileItem->getPath() << ": " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown error downloading remote file " << remoteFileItem->getPath() << std::endl;
        }
    }
}

void ParserYAML::buildAll() {
    buildFolders();
    buildFiles();
    buildRemoteFiles();
}

Variable* ParserYAML::findVariableByName(const std::string& name) const {
    for (const auto& var : m_variables) {
        if (toLower(var->getName()) == toLower(name)) {
            return var.get();
        }
    }
    return nullptr;
}

Prompt* ParserYAML::findPromptByName(const std::string& name) const {
    for (const auto& prompt : m_prompts) {
        if (toLower(prompt->getName()) == toLower(name)) {
            return prompt.get();
        }
    }
    return nullptr;
}

VariableType ParserYAML::parseVariableType(const std::string& typeStr) {
    std::string lowerType = toLower(typeStr);
    if (lowerType == "string") {
        return VariableType::vtString;
    }
    throw std::runtime_error("Unknown variable type: " + typeStr);
}

PromptType ParserYAML::parsePromptType(const std::string& typeStr) {
    std::string lowerType = toLower(typeStr);
    if (lowerType == "inputstring") {
        return PromptType::ptInputString;
    } else if (lowerType == "checklist") {
        return PromptType::ptChecklist;
    } else if (lowerType == "arraylist") {
        return PromptType::ptArrayList;
    }
    throw std::runtime_error("Unknown prompt input type: " + typeStr);
}

std::string ParserYAML::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace TemplateBuilder
