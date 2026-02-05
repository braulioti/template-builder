# Use Cases - Release v0.2.0

This document describes all test cases that must be validated for the new features in v0.2.0. All use cases from v0.1.0 remain valid and must continue to work.

## UC16 - Help Command

**Description**: The system must display comprehensive help information when the help command is invoked.

**Scenarios**:
- **UC16a**: Short form `-h`
- **UC16b**: Long form `--help`
- **UC16c**: Help with no arguments
- **UC16d**: Help with invalid arguments (should still show help)

**Preconditions**: Template Builder is installed and accessible in PATH.

**Steps**:
1. Execute: `TemplateBuilder -h` or `TemplateBuilder --help`
2. System displays help information

**Expected Result**: 
- Help text includes:
  - Application name and version
  - Description of the tool
  - List of all available commands
  - Description of each parameter
  - Usage examples
  - Formatting is readable and well-organized

**Test Commands**:
```bash
TemplateBuilder -h
TemplateBuilder --help
TemplateBuilder -h invalid-arg
```

---

## UC17 - List Available Samples

**Description**: The system must list all available template samples from the repository.

**Scenarios**:
- **UC17a**: List samples with short form `-l`
- **UC17b**: List samples with long form `--list`
- **UC17c**: List samples when repository is accessible
- **UC17d**: List samples when repository is not accessible (error handling)

**Preconditions**: 
- Template Builder is installed
- Network connection available (for UC17c)
- Repository is configured and accessible

**Steps**:
1. Execute: `TemplateBuilder -l` or `TemplateBuilder --list`
2. System connects to repository
3. System fetches sample metadata
4. System displays list of available samples

**Expected Result**: 
- List displays:
  - Sample names
  - Sample descriptions (if available)
  - Sample versions (if available)
  - Formatting is clear and readable
- Error message is displayed if repository is not accessible

**Test Commands**:
```bash
TemplateBuilder -l
TemplateBuilder --list
```

**Error Scenarios**:
- Network failure: Display user-friendly error message
- Repository not found: Display error with suggestion to check configuration
- Invalid response: Display error with suggestion to report issue

---

## UC18 - Download Sample from Repository

**Description**: The system must download a sample template from the repository.

**Scenarios**:
- **UC18a**: Download sample with short form `-r sample-name`
- **UC18b**: Download sample with long form `--repository sample-name`
- **UC18c**: Download existing sample
- **UC18d**: Download non-existent sample (error handling)
- **UC18e**: Download sample when network is unavailable (error handling)
- **UC18f**: Download sample that is a ZIP file (should extract automatically)

**Preconditions**: 
- Template Builder is installed
- Network connection available
- Repository is configured
- Sample exists in repository

**Steps**:
1. Execute: `TemplateBuilder -r sample-name` or `TemplateBuilder --repository sample-name`
2. System connects to repository
3. System downloads sample files
4. System extracts files if ZIP format
5. System saves sample to current directory or specified location

**Expected Result**: 
- Sample files are downloaded successfully
- If ZIP, files are extracted correctly
- Sample is ready to use (can be executed with TemplateBuilder)
- Progress feedback is shown during download
- Success message is displayed

**Test Commands**:
```bash
TemplateBuilder -r wordpress-theme
TemplateBuilder --repository sample
TemplateBuilder -r non-existent-sample  # Should show error
```

**Error Scenarios**:
- Sample not found: Display error "Sample 'sample-name' not found. Use -l to list available samples."
- Network failure: Display error with suggestion to check connection
- Download incomplete: Display error and clean up partial files
- Invalid ZIP: Display error if extraction fails

---

## UC19 - Extract ZIP File

**Description**: The system must extract ZIP archives to a specified directory.

**Scenarios**:
- **UC19a**: Extract ZIP with `--extract file.zip --output /path/to/dest`
- **UC19b**: Extract ZIP to current directory (default output)
- **UC19c**: Extract ZIP with nested directories
- **UC19d**: Extract corrupted ZIP (error handling)
- **UC19e**: Extract ZIP to non-existent directory (should create)
- **UC19f**: Extract ZIP with insufficient space (error handling)
- **UC19g**: Extract ZIP with permission issues (error handling)

**Preconditions**: 
- Template Builder is installed
- ZIP file exists and is accessible
- Sufficient disk space available
- Write permissions to destination directory

**Steps**:
1. Execute: `TemplateBuilder --extract archive.zip --output /path/to/destination`
2. System reads ZIP file
3. System creates destination directory if needed
4. System extracts all files preserving directory structure
5. System displays progress feedback

**Expected Result**: 
- All files are extracted correctly
- Directory structure is preserved
- File permissions are preserved where possible
- Progress feedback is shown for large archives
- Success message is displayed

**Test Commands**:
```bash
TemplateBuilder --extract sample.zip --output ./extracted
TemplateBuilder --extract sample.zip  # Extract to current directory
TemplateBuilder --extract corrupted.zip --output ./extracted  # Should show error
```

**Error Scenarios**:
- Corrupted ZIP: Display error "ZIP file is corrupted or invalid"
- Insufficient space: Display error with space requirements
- Permission denied: Display error with suggestion to check permissions
- Invalid destination: Display error if destination path is invalid

---

## UC20 - MSI Installation Detection

**Description**: The MSI installer must detect if Template Builder is already installed.

**Scenarios**:
- **UC20a**: Fresh installation (not installed)
- **UC20b**: Installation over existing same version
- **UC20c**: Installation over existing older version
- **UC20d**: Installation over existing newer version

**Preconditions**: 
- Windows operating system
- WiX Toolset installed
- MSI installer generated

**Steps**:
1. Run MSI installer
2. Installer checks for existing installation
3. Installer compares versions
4. Installer displays appropriate message

**Expected Result**: 
- **UC20a**: Normal installation proceeds
- **UC20b**: Offers repair/reinstall option
- **UC20c**: Suggests upgrade and proceeds with upgrade
- **UC20d**: Warns about downgrade and asks for confirmation

**Test Scenarios**:
1. Install v0.2.0 on clean system
2. Install v0.2.0 when v0.2.0 is already installed
3. Install v0.2.0 when v0.1.0 is installed
4. Install v0.1.0 when v0.2.0 is installed (downgrade)

---

## UC21 - MSI Upgrade Suggestion

**Description**: The MSI installer must suggest upgrade when a newer version is available.

**Scenarios**:
- **UC21a**: User has older version installed
- **UC21b**: User runs installer for newer version
- **UC21c**: User confirms upgrade
- **UC21d**: User cancels upgrade

**Preconditions**: 
- Template Builder v0.1.0 (or older) is installed
- MSI installer for v0.2.0 is available

**Steps**:
1. User runs v0.2.0 MSI installer
2. Installer detects v0.1.0 installation
3. Installer displays upgrade message
4. User chooses to proceed or cancel

**Expected Result**: 
- Clear message explaining upgrade is available
- Option to proceed with upgrade
- Option to cancel
- If proceeding: Uninstall old version and install new version
- If canceling: Installation is aborted

**Test Scenarios**:
1. Install v0.2.0 over v0.1.0 (accept upgrade)
2. Install v0.2.0 over v0.1.0 (cancel upgrade)
3. Verify old version is removed after upgrade
4. Verify new version works correctly after upgrade

---

## UC22 - Backward Compatibility - Existing YAML Processing

**Description**: All existing YAML template processing must continue to work as before.

**Preconditions**: 
- Template Builder v0.2.0 is installed
- Existing YAML templates from v0.1.0 are available

**Steps**:
1. Execute: `TemplateBuilder path/to/template.yaml`
2. System processes template as in v0.1.0

**Expected Result**: 
- All v0.1.0 use cases (UC01-UC15) continue to work
- No breaking changes in behavior
- YAML format compatibility maintained
- All template functions work as before

**Test Templates**: 
- `samples/sample.yaml`
- `samples/wordpress-theme.yaml`
- Any custom templates created for v0.1.0

---

## UC23 - Combined Commands

**Description**: The system must handle multiple commands appropriately.

**Scenarios**:
- **UC23a**: Help with other parameters (help should take precedence)
- **UC23b**: List with repository parameter (should show error or ignore)
- **UC23c**: Invalid parameter combinations

**Preconditions**: Template Builder is installed

**Steps**:
1. Execute command with multiple parameters
2. System processes according to priority rules

**Expected Result**: 
- Help (`-h`) takes precedence over other commands
- Conflicting parameters show appropriate error
- Clear error messages for invalid combinations

**Test Commands**:
```bash
TemplateBuilder -h -r sample  # Should show help
TemplateBuilder -l -r sample  # Should show error about conflicting parameters
TemplateBuilder --invalid-param  # Should show error
```

---

## UC24 - Repository Sample Structure

**Description**: Downloaded samples must have correct structure and be immediately usable.

**Scenarios**:
- **UC24a**: Sample contains single YAML file
- **UC24b**: Sample contains YAML file with referenced files
- **UC24c**: Sample contains complete directory structure

**Preconditions**: 
- Sample exists in repository
- Sample has valid structure

**Steps**:
1. Download sample: `TemplateBuilder -r sample-name`
2. Sample is extracted to current directory
3. User can immediately use the sample

**Expected Result**: 
- Sample files are in correct locations
- YAML file is valid and can be processed
- Referenced files are present
- Sample can be executed: `TemplateBuilder sample.yaml`

**Test Scenarios**:
1. Download simple sample (single YAML)
2. Download complex sample (with files directory)
3. Execute downloaded sample immediately
4. Verify all referenced files are present

---

## UC25 - Error Handling and User Feedback

**Description**: All new features must provide clear error messages and user feedback.

**Scenarios**:
- **UC25a**: Network errors during repository access
- **UC25b**: File system errors during extraction
- **UC25c**: Invalid command-line arguments
- **UC25d**: Missing required parameters

**Preconditions**: Various error conditions

**Steps**:
1. Trigger error condition
2. System handles error gracefully
3. System displays user-friendly error message

**Expected Result**: 
- Error messages are clear and actionable
- Suggestions are provided when possible
- No crashes or unhandled exceptions
- User can recover from error

**Test Scenarios**:
1. Disconnect network and try `-l` command
2. Try to extract to read-only directory
3. Use invalid parameter combinations
4. Use `-r` without sample name

---

## UC26 - YAML Version Validation and User Notification

**Description**: The system must validate the YAML template version against the installed Template Builder version and notify the user when the template version is higher than the supported version. This is an enhancement of UC02 from v0.1.0, adding version comparison with installed version and improved error messaging.

**Scenarios**:
- **UC26a**: Template version is lower than installed version (should work)
- **UC26b**: Template version equals installed version (should work)
- **UC26c**: Template version is higher than installed version (should show error)
- **UC26d**: Template version format is invalid (should show error)
- **UC26e**: Template version field is missing (should show error)

**Preconditions**: 
- Template Builder is installed (e.g., v0.2.0)
- YAML template file exists with version field
- Template version may be different from installed version

**Steps**:
1. Execute: `TemplateBuilder path/to/template.yaml`
2. System reads YAML file
3. System extracts version field from template
4. System compares template version with installed Template Builder version
5. System determines if template can be processed

**Expected Result**: 
- **UC26a/UC26b**: Template is processed normally, no error
- **UC26c**: System displays error message:
  - "Error: Template version X.X.X is not supported by Template Builder vY.Y.Y"
  - "Please update Template Builder to version X.X.X or higher to use this template"
  - System exits gracefully without processing template
- **UC26d**: System displays error for invalid version format
- **UC26e**: System displays error for missing version field

**Test Commands**:
```bash
# Template version 1.0.0, Template Builder v0.2.0 (should work)
TemplateBuilder template-v1.0.0.yaml

# Template version 2.0.0, Template Builder v0.2.0 (should show error)
TemplateBuilder template-v2.0.0.yaml

# Template version 0.1.0, Template Builder v0.2.0 (should work)
TemplateBuilder template-v0.1.0.yaml
```

**Error Message Format**:
```
ERROR: Template version incompatibility

Template version: 2.0.0
Template Builder version: 0.2.0

This template requires Template Builder version 2.0.0 or higher.
Please update Template Builder to use this template.

For download and installation instructions, visit:
https://github.com/braulioti/template-builder/releases
```

**Test Scenarios**:
1. Template v1.0.0 with Template Builder v0.2.0 (should work)
2. Template v2.0.0 with Template Builder v0.2.0 (should show error)
3. Template v0.1.0 with Template Builder v0.2.0 (should work)
4. Template with invalid version format (e.g., "abc") (should show error)
5. Template without version field (should show error from UC02c)
6. Verify error message includes both versions clearly
7. Verify error message includes actionable instruction
8. Verify system exits with appropriate error code

---

## UC27 - Template Details Parsing

**Description**: The system must parse and store template metadata from the `template` section in YAML files.

**Scenarios**:
- **UC27a**: Template with complete template section (all fields)
- **UC27b**: Template with partial template section (some fields missing)
- **UC27c**: Template without template section (backward compatibility)
- **UC27d**: Template with invalid email format
- **UC27e**: Template with invalid URL format

**Preconditions**: 
- Template Builder is installed
- YAML file exists with optional `template` section

**Steps**:
1. Execute: `TemplateBuilder path/to/template.yaml`
2. System reads YAML file
3. System checks for `template` section
4. System parses template metadata fields
5. System stores metadata for later use

**Expected Result**: 
- **UC27a**: All fields are parsed and stored correctly:
  - `template.name` → stored
  - `template.author` → stored
  - `template.email` → stored
  - `template.url` → stored
  - `template.installParameter` → stored
- **UC27b**: Only present fields are parsed, missing fields are optional
- **UC27c**: Template processes normally without template section (backward compatible)
- **UC27d/UC27e**: Invalid formats are accepted but may be validated (optional validation)

**Test YAML Examples**:
```yaml
# UC27a - Complete template section
version: 0.2
template:
  name: "WordPress Theme Template"
  author: "John Doe"
  email: "john.doe@example.com"
  url: "https://johndoe.com"
  installParameter: "wordpress-theme"
variables:
  # ... rest of template

# UC27b - Partial template section
version: 0.2
template:
  name: "Simple Template"
  author: "Jane Smith"
variables:
  # ... rest of template

# UC27c - No template section (backward compatible)
version: 0.2
variables:
  # ... rest of template
```

**Test Scenarios**:
1. Parse template with all fields present
2. Parse template with only name and author
3. Parse template without template section
4. Verify backward compatibility with existing templates
5. Verify optional fields don't cause errors when missing

---

## UC28 - Template Details Display

**Description**: The system must display template metadata when listing samples or showing template information.

**Scenarios**:
- **UC28a**: Display template details in sample listing (`-l` command)
- **UC28b**: Display template details when downloading sample (`-r` command)
- **UC28c**: Display template details during template processing (optional)
- **UC28d**: Handle templates without metadata gracefully

**Preconditions**: 
- Template Builder is installed
- Templates with template metadata exist
- Repository contains samples with metadata

**Steps**:
1. Execute: `TemplateBuilder -l` (for UC28a)
2. System fetches sample metadata from repository
3. System displays template information including:
   - Template name
   - Author name
   - Author email (if available)
   - Author URL (if available)
   - Install parameter

**Expected Result**: 
- **UC28a**: List displays template metadata in readable format:
  ```
  Available Templates:
  
  wordpress-theme
    Name: WordPress Theme Template
    Author: John Doe
    Email: john.doe@example.com
    URL: https://johndoe.com
    Install: TemplateBuilder -r wordpress-theme
  
  simple-template
    Name: Simple Template
    Author: Jane Smith
    Install: TemplateBuilder -r simple-template
  ```
- **UC28b**: Download command shows template info before downloading
- **UC28c**: Processing may optionally display template info
- **UC28d**: Templates without metadata show only install parameter

**Test Commands**:
```bash
TemplateBuilder -l
TemplateBuilder -r wordpress-theme
TemplateBuilder template-with-metadata.yaml
```

**Display Format Requirements**:
- Template name is clearly visible
- Author information is formatted nicely
- Email and URL are displayed if available
- Install parameter is shown for easy copy-paste
- Missing fields don't break the display
- Format is consistent and readable

**Test Scenarios**:
1. List samples with complete metadata
2. List samples with partial metadata
3. List samples without metadata (backward compatibility)
4. Verify formatting is readable and consistent
5. Verify install parameter is correctly displayed
6. Verify email and URL are clickable/accessible if possible
