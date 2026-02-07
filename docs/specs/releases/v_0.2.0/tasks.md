# Tasks - Release v0.2.0

## Command-Line Argument Service

### ArgumentParserService.hpp/cpp
- [x] Create ArgumentParserService class
- [x] Implement argument parsing (short and long forms)
- [x] Implement parameter validation
- [x] Implement help text generation
- [x] Implement routing to appropriate handlers (YAML path, help)
- [x] Add error handling for invalid arguments
- [x] Add support for `-h` / `--help`
- [ ] Add support for `-r` / `--repository <sample-name>`
- [ ] Add support for `-l` / `--list`
- [ ] Add unit tests for argument parsing
- [ ] Add unit tests for validation
- [ ] Add unit tests for help generation

## Repository Service

### RepositoryService.hpp/cpp
- [ ] Create RepositoryService class
- [ ] Implement repository URL configuration
- [ ] Implement sample metadata fetching
- [ ] Implement sample listing functionality
- [ ] Implement sample download functionality
- [ ] Add error handling for network failures
- [ ] Add error handling for invalid sample names
- [ ] Add progress feedback for downloads
- [ ] Add caching mechanism for metadata
- [ ] Add unit tests for listing
- [ ] Add unit tests for downloading
- [ ] Add integration tests with mock repository

## YAML-driven ZIP Extraction

### ExtractBuilder and ParseYAML integration
- [ ] Define YAML format for `extract` section (e.g. sequence of `zip` + `path` items)
- [ ] Add ExtractBuilder (or equivalent) to interpret extract entries and perform extraction
- [ ] In ParseYAML: add `loadExtracts(root)` and store extract items; call ExtractBuilder from `buildAll()`
- [ ] ExtractBuilder uses a ZIP library (minizip, libzip, etc.) to read and extract archives
- [ ] Implement extraction to directory (create destination if needed, preserve directory structure)
- [ ] Add error handling for corrupted files, insufficient space, permission issues
- [ ] Preserve file permissions where possible
- [ ] Add unit tests for ExtractBuilder
- [ ] Add unit tests for loadExtracts and buildAll with extract section

## Help System

- [ ] Design help text structure
- [ ] Implement help content generation
- [ ] Include all available commands
- [ ] Include parameter descriptions
- [ ] Include usage examples
- [ ] Format help text for readability
- [ ] Test help display on different terminal sizes
- [ ] Update help when new features are added

## YAML Version Validation Enhancement

### ParseYAML Service Updates
- [ ] Enhance ValidateVersion method to compare with installed version
- [ ] Implement version comparison logic (semantic versioning)
- [ ] Add detection for template version > installed version
- [ ] Create user-friendly error message for unsupported versions
- [ ] Include update instruction in error message
- [ ] Ensure error message is clear and actionable
- [ ] Add unit tests for version comparison
- [ ] Add unit tests for error message generation
- [ ] Test with various version scenarios:
  - [ ] Template version < installed version (should work)
  - [ ] Template version = installed version (should work)
  - [ ] Template version > installed version (should show error)
  - [ ] Invalid version format (should show error)

## Template Details Metadata

### TemplateDetails Type
- [x] Create TemplateDetails class/struct (DetailsBuilder exibe name, author, email, url, language)
- [ ] Define fields: name, author, email, url, installParameter (atual: name, author, email, url, language no YAML raiz)
- [ ] Implement getters and setters
- [ ] Add validation for email format (optional)
- [ ] Add validation for URL format (optional)
- [x] Make all fields optional for backward compatibility
- [ ] Add unit tests for TemplateDetails

### ParseYAML Service Updates for Template Details
- [x] Add LoadTemplateDetails method (displayTemplateMetadataIfApplicable)
- [ ] Parse `template` section from YAML (atual: name, author, email, url, language no raiz do YAML)
- [x] Extract template.name field (m_doc["name"])
- [x] Extract template.author field (m_doc["author"])
- [x] Extract template.email field (m_doc["email"])
- [x] Extract template.url field (m_doc["url"])
- [ ] Extract template.installParameter field
- [x] Handle missing template section (backward compatibility)
- [x] Handle missing optional fields gracefully
- [ ] Add unit tests for parsing template section
- [ ] Add unit tests for backward compatibility

### Repository Service Updates
- [ ] Include template metadata in sample listing
- [ ] Display author information in list output
- [ ] Display template name in list output
- [ ] Use installParameter for repository downloads
- [ ] Format metadata display in listing command
- [ ] Add unit tests for metadata display

### Display and Output
- [x] Create method to format template metadata (DetailsBuilder::displayAll)
- [x] Display template info during processing (optional)
- [ ] Include metadata in help/listing output
- [x] Format author information nicely
- [x] Handle missing metadata fields gracefully

## InputList Prompt Type

### PromptType and Parser
- [ ] Add `ptInputList` to PromptType enum (PromptType.hpp)
- [ ] In ParseYAML::parsePromptType, map `InputList` / `inputlist` to ptInputList
- [ ] Ensure InputList uses same `options` structure as Checklist/ArrayList (name/value)

### PromptBuilder
- [ ] Implement getInputList (or equivalent) in PromptBuilder: display options, single selection, set variable to selected option value
- [ ] In processPromptInput, handle PromptType::ptInputList and call getInputList
- [ ] Non-interactive mode: same behavior as other prompts (default/empty when stdin not interactive)

### Tests and validation
- [ ] Add unit tests for parsePromptType with InputList
- [ ] Add unit tests for InputList prompt (single selection, variable value)
- [ ] Validate UC29 - InputList use case (single selection from list)

## Project Logo and Branding

- [ ] Create project logo (asset suitable for favicon, project image, and ASCII conversion)
- [ ] Add logo as favicon in the Angular frontend (e.g. index.html / angular.json assets)
- [ ] Use logo as project/repository image and in branding assets (e.g. Open Graph)
- [ ] Add ASCII art version of the logo to the CLI startup banner (e.g. in template-builder.cpp)
- [ ] Ensure logo displays correctly in terminal (ASCII art) and in browser (favicon)

## Frontend - Refactor component CSS

- [ ] Refactor CSS/SCSS currently in component files (e.g. `*.component.scss` in `app/components` and `app/pages`)
- [ ] Identify shared styles and move to global styles (e.g. `src/styles/`) or shared partials
- [ ] Consolidate variables, mixins, and common layout/theme rules to avoid duplication
- [ ] Keep component-scoped styles only where encapsulation is required; move reusable rules to shared stylesheets
- [ ] Ensure build and visual appearance remain correct after refactor
- [ ] Update stylelint/Prettier if needed for new structure

## Main Program Updates

### main.cpp updates
- [x] Integrate ArgumentParserService
- [x] Add routing for `-h` / `--help` command
- [ ] Add routing for `-l` / `--list` command
- [ ] Add routing for `-r` / `--repository` command
- [x] Maintain existing YAML processing flow (extraction is triggered by YAML `extract` section, not CLI)
- [x] Update error messages
- [x] Add version display
- [ ] Integrate enhanced version validation
- [ ] Test all command combinations

## MSI Installer Enhancements

### WiX Configuration Updates
- [ ] Update TemplateBuilder.wxs to detect existing installation
- [ ] Implement version comparison logic
- [ ] Add upgrade detection
- [ ] Add repair/reinstall option for same version
- [ ] Add upgrade suggestion for older versions
- [ ] Add downgrade warning for newer versions
- [ ] Update installer UI messages
- [ ] Test installation scenarios:
  - [ ] Fresh installation
  - [ ] Upgrade from older version
  - [ ] Reinstall same version
  - [ ] Attempt to install older version over newer

### build-msi.ps1 Updates
- [ ] Add version detection logic
- [ ] Update installer generation to include version info
- [ ] Test installer generation with version detection

## Testing and Validation

### Unit Tests
- [ ] Test ArgumentParserService with all parameter combinations
- [ ] Test RepositoryService listing functionality
- [ ] Test RepositoryService download functionality
- [ ] Test ZipExtractorService with various ZIP files
- [ ] Test help system output
- [ ] Test error handling in all new services

### Integration Tests
- [ ] Test complete flow: `-l` command
- [ ] Test complete flow: `-r sample-name` command
- [ ] Test complete flow: `-h` command
- [ ] Test complete flow: YAML template with `extract` section (ExtractBuilder runs during buildAll)
- [ ] Test backward compatibility with existing YAML processing
- [ ] Test error scenarios (network failures, invalid files, etc.)

### Platform Testing
- [ ] Test on Windows (MSVC)
- [ ] Test on Windows (MinGW)
- [ ] Test on Linux (GCC)
- [ ] Test on Linux (Clang)
- [ ] Test on macOS (Clang)
- [ ] Test ZIP extraction on all platforms
- [ ] Test repository download on all platforms
- [ ] Test MSI installer on Windows

### Use Cases Validation
- [ ] Run all use cases from use-cases.md
- [ ] Validate UC16 - Help Command
- [ ] Validate UC17 - List Samples
- [ ] Validate UC18 - Download Sample
- [ ] Validate UC19 - Extract ZIP File
- [ ] Validate UC20 - MSI Installation Detection
- [ ] Validate UC21 - MSI Upgrade Suggestion
- [ ] Validate UC26 - YAML Version Validation
- [ ] Validate UC27 - Template Details Parsing
- [ ] Validate UC28 - Template Details Display
- [ ] Validate UC29 - InputList prompt (single selection from list)
- [ ] Validate project logo: favicon in web app, project image, ASCII art in CLI

## Documentation

- [ ] Update README.md with new CLI parameters
- [ ] Document repository structure
- [ ] Document sample download process
- [ ] Document YAML `extract` section and ExtractBuilder (ZIP extraction feature)
- [ ] Update installation guide with MSI upgrade info
- [ ] Create examples for new features
- [ ] Update CHANGELOG.md
- [ ] Document API for new services (if applicable)

## Repository Setup

- [ ] Define repository structure for samples
- [ ] Create sample metadata format (JSON/YAML)
- [ ] Organize existing samples in repository structure
- [ ] Create sample index/manifest file
- [ ] Set up sample hosting (GitHub releases, raw files, or CDN)
- [ ] Test sample download from repository
- [ ] Create documentation for adding new samples

## Build and Distribution

- [ ] Update build scripts for new dependencies
- [ ] Test build on all platforms with new dependencies
- [ ] Update CI/CD workflows for new dependencies
- [ ] Test MSI installer generation with new features
- [ ] Test generated binaries on clean systems
- [ ] Prepare release notes for v0.2.0

## Cleanup and Finalization

- [ ] Review all new code
- [ ] Apply consistent formatting (clang-format)
- [ ] Remove commented/debug code
- [ ] Validate no memory leaks
- [ ] Performance testing for download and extraction
- [x] Review and update tasks.md marking completed
- [ ] Prepare PR/Merge for main branch
