# Release v0.2.0 - Enhanced CLI and Repository Support

## Release Objective

This release focuses on enhancing the Template Builder CLI with repository support, improved command-line interface, and better installation management. The main goals are to enable users to download samples from a repository, list available templates, and improve the overall user experience with better help documentation and installation handling.

## Scope

### New Features

#### 1. Repository Sample Download
- **Feature**: Download samples directly from the repository
- **Parameters**: `-r` or `--repository <sample-name>`
- **Description**: Allows users to download pre-configured template samples from the repository without manually cloning or copying files
- **Use Case**: Users can quickly get started with templates by downloading official samples

#### 2. Sample Listing
- **Feature**: List available samples in the repository
- **Parameters**: `-l` or `--list`
- **Description**: Displays all available template samples that can be downloaded from the repository
- **Use Case**: Users can discover available templates before downloading

#### 3. Help System
- **Feature**: Comprehensive help documentation
- **Parameters**: `-h` or `--help`
- **Description**: Displays detailed help information including all available commands, parameters, and usage examples
- **Use Case**: Users can quickly understand how to use the tool without external documentation

#### 4. Command-Line Argument Service
- **Feature**: Dedicated service for handling command-line arguments
- **Description**: A new service class will be created to parse, validate, and manage all CLI arguments
- **Benefits**: 
  - Centralized argument handling
  - Better error messages
  - Easier to extend with new parameters
  - Improved code organization

#### 5. ZIP File Extraction (YAML-driven)
- **Feature**: Extract ZIP archives as defined in the YAML template
- **Description**: A new `extract` section in the YAML lists ZIP files and destination paths. A dedicated builder (e.g. **ExtractBuilder**) interprets these entries during template processing and performs the extraction. There is no separate CLI command for extraction.
- **YAML structure**: An `extract` key containing a sequence of items, each with a zip source (path or URI) and a destination `path`. The parser loads these entries and the ExtractBuilder executes each extraction (creating destination directories as needed, preserving structure).
- **Use Case**: Handle compressed template packages and sample distributions within the same template run as files, folders, and remote files.

#### 6. MSI Installer Enhancement
- **Feature**: Installation detection and upgrade suggestion
- **Description**: 
  - Detect if Template Builder is already installed
  - Suggest upgrade when a newer version is available
  - Provide clear messaging during installation
- **Benefits**: Better user experience during installation and updates

#### 7. YAML Version Validation Enhancement
- **Feature**: Enhanced version validation with user notification
- **Description**: 
  - Validate YAML template version against installed Template Builder version
  - Detect when YAML version is higher than supported version
  - Notify user that template cannot be interpreted
  - Provide clear message instructing user to update Template Builder
- **Benefits**: 
  - Prevents confusion when templates fail to process
  - Guides users to update their installation
  - Better error messages for version incompatibility

#### 8. Template Details Metadata
- **Feature**: Template metadata and author information
- **Description**: 
  - Add `template` section to YAML format with metadata
  - Include author name, email, and website URL
  - Display template information during processing or listing
- **YAML Structure**:
  ```yaml
  version: 0.2
  template:
    name: "Template Name"
    author: "Author Name"
    email: "author@example.com"
    url: "https://author-website.com"
    language: "pt-BR"
  ```
- **Benefits**: 
  - Better template organization and identification
  - Attribution for template creators
  - Easier template discovery and management
  - Support for repository-based template installation

#### 9. InputList Prompt Type
- **Feature**: New prompt input type for single-selection from a list
- **Description**: **InputList** allows the user to choose one item from a list of options (unlike Checklist, which allows multiple selections). The selected option’s value is stored in the associated variable.
- **YAML structure**: Same `inputs` structure as existing prompts, with `type: InputList` and `options` (list of `name`/`value`). One option is selected by the user; the variable receives that option’s `value`.
- **Use Case**: License type, single framework choice, theme variant, etc.
- **Example**:
  ```yaml
  inputs:
    - variable: license
      input: "Choose license: "
      type: InputList
      options:
        - name: "MIT"
          value: "MIT"
        - name: "GPL v2"
          value: "GPL-2.0"
        - name: "Apache 2.0"
          value: "Apache-2.0"
  ```

#### 10. Frontend CSS refactor
- **Feature**: Refactor CSS/SCSS in component files
- **Description**: Refactor the styles that currently live in component files (`*.component.scss` in `app/components` and `app/pages`). Consolidate shared styles into global or shared stylesheets (e.g. under `src/styles/`), reduce duplication (variables, mixins, common layout/theme), and keep component-scoped styles only where encapsulation is needed.
- **Benefits**: Easier maintenance, consistent theming, smaller bundle where shared styles are reused.

#### 11. Project Logo and Branding
- **Feature**: Create and integrate the project logo across the product
- **Description**: In this version the project logo will be created and added in three forms:
  - **Favicon**: Use the logo as the favicon of the web application (Angular frontend)
  - **Project image**: Use the logo as the project/social image (e.g. repository image, Open Graph, branding assets)
  - **ASCII Art in the CLI**: Display the logo as ASCII art when the CLI starts (e.g. in the banner shown by the main program)
- **Benefits**: 
  - Consistent visual identity
  - Recognizable branding in the CLI, web UI, and repository
  - Improved professional appearance

## Technologies and Tools

### New Dependencies

- **ZIP Library**: A C++ library for handling ZIP file extraction (e.g., minizip, libzip, or similar)
- **HTTP Client**: For downloading samples from repository (e.g., libcurl, cpp-httplib, or similar)
- **Argument Parser**: Enhanced CLI argument parsing (can use existing C++17 features or a lightweight library like cxxopts)

### Target Platforms

- Windows (x64) - MSVC and MinGW
- Linux (x64) - GCC and Clang
- macOS (x64 and ARM64) - Clang

## Compatibility

### Maintained Features

All features from v0.1.0 are maintained:
- ✅ YAML file parsing version 1.0
- ✅ Variable management (string type)
- ✅ Interactive prompts (InputString, Checklist, ArrayList, InputList)
- ✅ File generation (static and dynamic)
- ✅ Directory creation
- ✅ Template functions (upper, lower, replace)
- ✅ Nested function support
- ✅ UTF-8 encoding

### New YAML Format Extensions

- ✅ Template Details section with metadata:
  - `template.name`: Display name of the template
  - `template.author`: Author name
  - `template.email`: Author email address
  - `template.url`: Author website URL

### Backward Compatibility

- Existing YAML templates remain fully compatible
- Existing CLI usage (without new parameters) continues to work
- No breaking changes to the core template processing logic
- Templates with version higher than installed version will be rejected with clear error message

## Architecture Changes

### New Service: ArgumentParserService

A dedicated service will be created to handle all command-line arguments:

```cpp
class ArgumentParserService {
    // Parse command-line arguments
    // Validate parameters
    // Provide help information
    // Route to appropriate handlers
};
```

### Repository Structure

The repository will need to support:
- Sample templates organized in a structured format
- Metadata about available samples
- Download endpoints or file locations

## Success Criteria

1. ✅ All new CLI parameters work correctly (`-r`, `-l`, `-h`)
2. ✅ Repository sample download functionality is implemented
3. ✅ Sample listing displays all available templates
4. ✅ Help system provides comprehensive documentation
5. ✅ ZIP extraction works on all target platforms
6. ✅ MSI installer detects existing installations
7. ✅ MSI installer suggests upgrades appropriately
8. ✅ YAML version validation detects unsupported versions
9. ✅ User notification for version incompatibility is clear and actionable
10. ✅ Template Details metadata is parsed and stored
11. ✅ Template metadata is displayed in listing and help
12. ✅ All existing functionality remains intact
13. ✅ Unit tests for new features
14. ✅ Documentation updated
15. ✅ Project logo created and integrated as favicon (web), project image, and ASCII art (CLI)
16. ✅ InputList prompt type implemented (single selection from list)
17. ✅ Frontend component CSS refactored (shared styles consolidated, duplication reduced)

## Implementation Notes

### Command-Line Interface

The CLI will support the following syntax:

```bash
# Download a sample from repository
TemplateBuilder -r sample-name
TemplateBuilder --repository sample-name

# List available samples
TemplateBuilder -l
TemplateBuilder --list

# Show help
TemplateBuilder -h
TemplateBuilder --help

# Extract ZIP file
TemplateBuilder template.yaml   # template includes extract section; ExtractBuilder performs extraction

# Existing usage (still supported)
TemplateBuilder path/to/template.yaml
```

### MSI Installer Behavior

- Check registry or installation directory for existing installation
- Compare version numbers
- Display appropriate message:
  - If not installed: Normal installation
  - If same version: Offer repair/reinstall
  - If older version: Suggest upgrade
  - If newer version: Warn about downgrade

### ZIP Extraction

- Support standard ZIP format
- Handle nested directories
- Preserve file permissions where possible
- Provide progress feedback for large archives
- Handle errors gracefully (corrupted files, insufficient space, etc.)

### YAML Version Validation

- Compare template version with installed Template Builder version
- If template version > installed version:
  - Display error message: "Template version X.X.X is not supported by Template Builder vY.Y.Y"
  - Display instruction: "Please update Template Builder to version X.X.X or higher to use this template"
  - Exit gracefully without processing template
- If template version <= installed version:
  - Process template normally
- Error message should be clear, user-friendly, and actionable

### Template Details Format

The YAML format now supports an optional `template` section with metadata:

```yaml
version: 0.2
template:
  name: "WordPress Theme Template"
  author: "John Doe"
  email: "john.doe@example.com"
  url: "https://johndoe.com"
variables:
  # ... rest of template
```

**Field Descriptions**:
- `template.name`: Human-readable name of the template (optional)
- `template.author`: Name of the template author (optional)
- `template.email`: Email address of the author (optional)
- `template.url`: Website URL of the author (optional)

**Usage**:
- Template metadata is displayed when listing samples (`-l` command)
- Metadata is shown in repository sample information
- All fields are optional for backward compatibility
- Templates without `template` section continue to work normally

### InputList Prompt Type

- **PromptType**: Add `ptInputList` (or equivalent) to the enum; parser maps YAML `InputList` to this type
- **YAML**: Same `options` format as Checklist/ArrayList (sequence of `name`/`value`). Required for InputList
- **CLI behavior**: Display the prompt text and the list of options; user selects one item (e.g. by number or arrow keys); store the selected option’s `value` in the variable
- **Backward compatibility**: Existing templates unchanged; InputList is additive

### Frontend CSS refactor

- **Scope**: Component SCSS in `app/components` (header, menu, main, sidebar, footer) and `app/pages` (build-template-*, home, changelog, installation, terms-of-use, using-template)
- **Target**: Shared styles (variables, mixins, layout, theme) moved to `src/styles/` or similar; component files keep only styles that must be encapsulated
- **Validation**: Build succeeds, visual regression check (or manual verification) after refactor

### Project Logo and Branding

- **Logo asset**: Create the project logo (e.g. SVG/PNG) suitable for favicon, social/repo image, and ASCII conversion
- **Favicon**: Add the logo as favicon in the Angular app (e.g. in `index.html` and/or `angular.json` assets)
- **Project image**: Use the logo as the repository/project image and in Open Graph or other branding assets
- **CLI ASCII art**: Replace or augment the current text banner in the CLI (e.g. in `template-builder.cpp`) with an ASCII art version of the logo shown at startup

## Timeline

*To be defined based on team availability*

## Migration Notes

- No migration required for existing users
- New parameters are optional and don't affect existing workflows
- Repository samples will be available as additional resources
- Existing templates continue to work without modification

## Future Considerations

- Support for private repositories
- Template validation before download
- Sample templates versioning
- Template marketplace or community contributions
- Integration with package managers (vcpkg, Conan, etc.)
