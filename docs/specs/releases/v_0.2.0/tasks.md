# Tasks - Release v0.2.0

## Setup and Infrastructure

- [ ] Research and select ZIP library (minizip, libzip, or similar)
- [ ] Research and select HTTP client library (libcurl, cpp-httplib, or similar)
- [ ] Research and select argument parser library (cxxopts or implement custom)
- [ ] Add new dependencies to CMakeLists.txt
- [ ] Configure dependency management (FetchContent or package manager)
- [ ] Update CI/CD to include new dependencies
- [ ] Create directory structure for new services

## Command-Line Argument Service

### ArgumentParserService.hpp/cpp
- [ ] Create ArgumentParserService class
- [ ] Implement argument parsing (short and long forms)
- [ ] Implement parameter validation
- [ ] Implement help text generation
- [ ] Implement routing to appropriate handlers
- [ ] Add error handling for invalid arguments
- [ ] Add support for `-h` / `--help`
- [ ] Add support for `-r` / `--repository <sample-name>`
- [ ] Add support for `-l` / `--list`
- [ ] Add support for `--extract <zip-file> --output <path>`
- [ ] Maintain backward compatibility with existing usage
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

## ZIP Extraction Service

### ZipExtractorService.hpp/cpp
- [ ] Create ZipExtractorService class
- [ ] Implement ZIP file reading
- [ ] Implement file extraction to directory
- [ ] Implement nested directory extraction
- [ ] Implement progress feedback
- [ ] Add error handling for corrupted files
- [ ] Add error handling for insufficient space
- [ ] Add error handling for permission issues
- [ ] Preserve file permissions where possible
- [ ] Add support for large files
- [ ] Add unit tests for extraction
- [ ] Add unit tests for error handling
- [ ] Add tests for nested directories

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
- [ ] Create TemplateDetails class/struct
- [ ] Define fields: name, author, email, url, installParameter
- [ ] Implement getters and setters
- [ ] Add validation for email format (optional)
- [ ] Add validation for URL format (optional)
- [ ] Make all fields optional for backward compatibility
- [ ] Add unit tests for TemplateDetails

### ParseYAML Service Updates for Template Details
- [ ] Add LoadTemplateDetails method
- [ ] Parse `template` section from YAML
- [ ] Extract template.name field
- [ ] Extract template.author field
- [ ] Extract template.email field
- [ ] Extract template.url field
- [ ] Extract template.installParameter field
- [ ] Handle missing template section (backward compatibility)
- [ ] Handle missing optional fields gracefully
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
- [ ] Create method to format template metadata
- [ ] Display template info during processing (optional)
- [ ] Include metadata in help/listing output
- [ ] Format author information nicely
- [ ] Handle missing metadata fields gracefully

## Main Program Updates

### main.cpp updates
- [ ] Integrate ArgumentParserService
- [ ] Add routing for `-h` / `--help` command
- [ ] Add routing for `-l` / `--list` command
- [ ] Add routing for `-r` / `--repository` command
- [ ] Add routing for `--extract` command
- [ ] Maintain existing YAML processing flow
- [ ] Update error messages
- [ ] Add version display
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
- [ ] Test complete flow: `--extract` command
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

## Documentation

- [ ] Update README.md with new CLI parameters
- [ ] Document repository structure
- [ ] Document sample download process
- [ ] Document ZIP extraction feature
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
- [ ] Review and update tasks.md marking completed
- [ ] Prepare PR/Merge for main branch
