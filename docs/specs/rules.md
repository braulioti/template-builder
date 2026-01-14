# Template Builder - Project Specification

## Overview

Template Builder is a CLI tool that allows users to create custom project templates from YAML configuration files. The tool generates ready-made project structures by creating folders, generating files, and collecting user input through interactive CLI prompts defined in YAML files.

## Core Functionality

### Current Capabilities

1. **YAML-based Template Definition**: Templates are defined using YAML files with a version field (currently supports version 1.0)

2. **Variable Management**: 
   - Define variables with types (currently supports string type)
   - Variables can have default values
   - Variables are used in file content templating

3. **Interactive CLI Prompts**:
   - **InputString**: Simple text input prompts
   - **Checklist**: Multiple choice selections with checkboxes
   - **ArrayList**: List-based selection interface
   - Prompts collect user input and populate variables

4. **File Generation**:
   - Create files with static content
   - Create files using prompts (dynamic content based on user input)
   - Support for template variables in file content using `{{variableName}}` syntax
   - Support for template functions: `upper()`, `lower()`, `replace()`
   - Nested function calls supported
   - Automatic directory creation for file paths

5. **Folder Creation**:
   - Create directory structures as specified in YAML
   - Automatic parent directory creation

### Technical Stack

- **Language**: Delphi/Object Pascal
- **Platform**: Windows (Console Application)
- **YAML Library**: VSoft.YAML
- **Current Version**: 1.0

### Project Structure

```
src/
├── builders/          # Components that build project artifacts
│   ├── unFileBuilder.pas      # Handles file creation
│   ├── unFolderBuilder.pas    # Handles folder creation
│   ├── unPromptBuilder.pas    # Handles interactive prompts and templating
│   └── unVariableBuilder.pas  # Variable management (if exists)
├── services/          # Core services
│   └── unParseYAML.pas        # YAML parsing and orchestration
├── types/             # Type definitions
│   ├── unFileType.pas         # File data structures
│   ├── unPromptType.pas       # Prompt-related types
│   └── unVariableType.pas     # Variable type definitions
└── TemplateBuilder.dpr        # Main program entry point
```

## YAML Template Format

### Structure

```yaml
version: 1.0

variables:
  - name: variableName
    type: string
    value: defaultValue  # optional

prompts:
  - name: promptName
    inputs:
      - variable: variableName
        input: "Prompt text: "
        type: InputString  # or Checklist, ArrayList
        options: []  # for Checklist/ArrayList types
    result: |
      Template content with {{variableName}} and {{upper(variableName)}}

files:
  - path: "output/file.txt"
    content: "Static content"
  - path: "output/generated.txt"
    prompt: promptName  # Use prompt result as content

folders:
  - path: "output/subdirectory"
```

## Usage

```
templatebuilder.exe <arquivo.yaml>
```

The tool:
1. Parses the YAML file
2. Validates the version
3. Loads variables, prompts, files, and folders
4. Executes prompts to collect user input
5. Generates files and folders in the current working directory
6. Provides console output showing created files and folders

## Future Capabilities (Mentioned but Not Yet Implemented)

Based on the project description, the following features are planned or mentioned:
- **Download files**: Ability to download files from URLs
- **Extract zip files**: Ability to extract zip archives

## Development Guidelines

When working on this project:
- Follow Delphi/Object Pascal coding conventions
- Maintain backward compatibility with YAML version 1.0
- Ensure proper error handling and user-friendly error messages
- All file operations should be relative to the current working directory
- Support UTF-8 encoding for file content
- Template functions should be extensible for future additions

## Notes

- The tool uses a builder pattern for constructing files, folders, and prompts
- Variable resolution supports nested function calls
- File paths in YAML are relative to the current working directory
- The tool automatically creates parent directories as needed
- All prompts are executed before file generation begins