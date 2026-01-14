# Use Cases - Release v0.1.0

This document describes all test cases that must be validated after converting the project to C++.

## UC01 - Basic YAML Parsing

**Description**: The system must be able to parse a valid YAML file with basic structure.

**Preconditions**: Valid YAML file exists in the file system.

**Steps**:
1. Execute: `templatebuilder <file.yaml>`
2. Verify that the system parses the file without errors

**Expected Result**: File is parsed successfully, without parsing errors.

**Test Templates**: `samples/sample.yaml`, `samples/wordpress-theme.yaml`

---

## UC02 - Version Validation

**Description**: The system must validate the YAML template version.

**Scenarios**:
- **UC02a**: Valid version 1.0
- **UC02b**: Unsupported version should generate error
- **UC02c**: Missing version field should generate error

**Expected Result**:
- UC02a: Accepts and processes
- UC02b: Error "Template version not supported"
- UC02c: Error "Required field 'version' not found"

---

## UC03 - Variable Loading

**Description**: The system must load variables defined in YAML.

**Steps**:
1. YAML contains `variables` section with variables
2. System loads all variables
3. Variables can have default values

**Expected Result**: All variables are loaded correctly, default values are applied when specified.

---

## UC04 - Prompts - InputString

**Description**: The system must display a simple text prompt and collect user input.

**Steps**:
1. YAML defines InputString type prompt
2. System displays prompt text
3. User types value
4. Value is stored in associated variable

**Expected Result**: Typed value is correctly stored and available for use in templates.

---

## UC05 - Prompts - Checklist

**Description**: The system must display an interactive checklist and allow multiple selections.

**Steps**:
1. YAML defines Checklist type prompt with options
2. System displays options with checkboxes
3. User navigates and selects options (space to check/uncheck)
4. User confirms selection (Enter)
5. Selected values are stored

**Expected Result**: Multiple selections are correctly collected and stored.

---

## UC06 - Prompts - ArrayList

**Description**: The system must display a list of options and allow single selection.

**Steps**:
1. YAML defines ArrayList type prompt with options
2. System displays list of options
3. User navigates the list (arrow keys)
4. User selects an option (Enter)
5. Selected value is stored

**Expected Result**: Single selection is correctly collected and stored.

---

## UC07 - Directory Creation

**Description**: The system must create directory structure as specified in YAML.

**Scenarios**:
- **UC07a**: Create simple directory
- **UC07b**: Create nested directory structure
- **UC07c**: Directory already exists (should not generate error)

**Expected Result**: All directories specified in `folders` are created correctly.

---

## UC08 - File Generation - Static Content

**Description**: The system must create files with static content defined in YAML.

**Steps**:
1. YAML defines file in `files` with `content`
2. System creates file at specified path
3. Content is written to file

**Expected Result**: File is created with exact content as specified in YAML.

---

## UC09 - File Generation - With Template Variables

**Description**: The system must substitute variables in templates using `{{variable}}` syntax.

**Steps**:
1. File has content with `{{variableName}}`
2. Variable was filled previously (via prompt or default value)
3. System substitutes placeholder with variable value

**Expected Result**: All `{{variableName}}` are substituted with correct values.

---

## UC10 - File Generation - With Template Functions (upper)

**Description**: The system must process `upper()` function in templates.

**Steps**:
1. File has content with `{{upper(variableName)}}`
2. System processes upper function
3. Value is converted to uppercase

**Expected Result**: upper() function returns string in uppercase.

---

## UC11 - File Generation - With Template Functions (lower)

**Description**: The system must process `lower()` function in templates.

**Steps**:
1. File has content with `{{lower(variableName)}}`
2. System processes lower function
3. Value is converted to lowercase

**Expected Result**: lower() function returns string in lowercase.

---

## UC12 - File Generation - With Template Functions (replace)

**Description**: The system must process `replace()` function in templates.

**Steps**:
1. File has content with `{{replace("old", "new", variableName)}}`
2. System processes replace function
3. Occurrences are replaced

**Expected Result**: replace() function replaces all occurrences correctly.

---

## UC13 - File Generation - Nested Functions

**Description**: The system must support nested functions in templates.

**Example**: `{{upper(replace(" ", "_", variableName))}}`

**Expected Result**: Nested functions are processed correctly, from inside out.

---

## UC14 - File Generation - Using Prompt Result

**Description**: The system must use prompt result as file content.

**Steps**:
1. YAML defines prompt with `result`
2. File in `files` references prompt via `prompt: promptName`
3. System executes prompt, collects inputs
4. System generates content using prompt's `result`
5. Generated content is written to file

**Expected Result**: File is created with content generated from prompt result, with all substitutions applied.

---

## UC15 - Automatic Parent Directory Creation

**Description**: The system must automatically create parent directories when creating files.

**Steps**:
1. YAML defines file with path `output/subdir/file.txt`
2. Directory `output/subdir` does not exist
3. System automatically creates necessary directories

**Expected Result**: Parent directories are automatically created before creating the file.

---

## UC16 - UTF-8 Encoding

**Description**: The system must support UTF-8 encoding in generated files.

**Steps**:
1. Content contains UTF-8 characters (accents, special characters)
2. System creates file
3. File is saved with UTF-8 encoding

**Expected Result**: File is saved correctly with UTF-8 encoding, special characters preserved.

---

## UC17 - Error Handling - YAML File Not Found

**Description**: System must display friendly error when YAML file does not exist.

**Steps**:
1. Execute: `templatebuilder nonexistent_file.yaml`
2. Verify error message

**Expected Result**: Clear message: "File not found: nonexistent_file.yaml"

---

## UC18 - Error Handling - Invalid YAML

**Description**: System must display error when YAML is malformed.

**Expected Result**: Clear error message indicating YAML parsing problem.

---

## UC19 - Error Handling - Undefined Variable in Prompt

**Description**: System must detect when prompt references non-existent variable.

**Expected Result**: Clear error indicating variable not found.

---

## UC20 - Complete End-to-End Flow

**Description**: Execute complete template with all functionalities.

**Steps**:
1. Use complex template (e.g., wordpress-theme.yaml)
2. Answer all prompts
3. Verify that all files and directories are created
4. Verify content of generated files

**Expected Result**: Complete template is processed successfully, all files and directories created correctly, valid content in all files.

---

## UC21 - Usage - No Arguments

**Description**: System must display usage when executed without arguments.

**Steps**:
1. Execute: `templatebuilder`
2. Verify usage message

**Expected Result**: Displays message: "Use: templatebuilder.exe <file.yaml>"

---

## UC22 - Version Banner

**Description**: System must display banner with version and author information.

**Expected Result**: Banner is displayed with correct information (version, author, description).

---

## UC23 - Success Message

**Description**: System must display success message at end of execution.

**Expected Result**: Message "Template successfully generated." is displayed.

---

## UC24 - Multiple Files and Directories

**Description**: System must process multiple files and directories in a single template.

**Expected Result**: All files and directories are created correctly, in appropriate order.

---

## UC25 - Variables with Default Values

**Description**: Variables with default values must work correctly.

**Steps**:
1. YAML defines variable with `value: defaultValue`
2. Prompt is not executed for this variable (or user does not provide value)
3. Default value is used

**Expected Result**: Default value is used when variable is not filled via prompt.