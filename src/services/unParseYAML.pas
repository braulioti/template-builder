unit unParseYAML;

interface

uses
  System.SysUtils, System.Generics.Collections, VSoft.YAML, StrUtils,
  unFileType, unFileBuilder, unPromptType, unVariableType, VSoft.YAML.Classes;

type
  EUnsupportedTemplateVersion = class(Exception);

type TParserYAML = class
private
  FVersion: string;
  FDoc: IYAMLDocument;
  FFiles: TObjectList<TFileData>;
  FVariables: TObjectList<TVariable>;
  FPrompts: TObjectList<TPrompt>;
  FFileBuilder: TFileBuilder;
  procedure ValidateVersion;
  procedure LoadFiles;
  procedure LoadVariables;
  procedure LoadPrompts;
  procedure Initializer;
public
  constructor Create(const FileName: string);
  destructor Destroy; override;
  procedure BuildAll;

  property Version: string read FVersion;
  property Document: IYAMLDocument read FDoc;
  property Files: TObjectList<TFileData> read FFiles;
  property Variables: TObjectList<TVariable> read FVariables;
  property Prompts: TObjectList<TPrompt> read FPrompts;
end;

implementation

const
  SUPPORTED_VERSIONS: array [0..0] of string = ('1.0');

{ TParseYAML }


constructor TParserYAML.Create(const FileName: string);
var
  VersionNode: IYAMLValue;
begin
  if FileName.IsEmpty then
    raise Exception.Create('YAML file not provided.');

  if not FileExists(FileName) then
    raise Exception.Create('YAML file not found: ' + FileName);

  // Load YAML
  FDoc := TYAML.LoadFromFile(FileName);

  // Access 'version' key
  VersionNode := FDoc.Root.Values['version'];
  if VersionNode = nil then
    raise Exception.Create('Required field "version" not found in YAML.');

  FVersion := FDoc.Root.Values['version'].AsString;

  ValidateVersion;
  Initializer;
  LoadVariables;
  LoadPrompts;
  LoadFiles;
end;

destructor TParserYAML.Destroy;
begin
  if Assigned(FFileBuilder) then
    FFileBuilder.Free;
  if Assigned(FVariables) then
    FVariables.Free;
  if Assigned(FPrompts) then
    FPrompts.Free;
  if Assigned(FFiles) then
    FFiles.Free;
  inherited;
end;

procedure TParserYAML.Initializer;
begin
  FFiles := TObjectList<TFileData>.Create(True);
  FVariables := TObjectList<TVariable>.Create(True);
  FPrompts := TObjectList<TPrompt>.Create(True);
  FFileBuilder := TFileBuilder.Create;
end;

procedure TParserYAML.LoadVariables;
var
  VariablesNode: IYAMLValue;
  VariableArray: IYAMLSequence;
  VariableItem: IYAMLValue;
  VariableObj: TVariable;
  TypeStr: string;
  i: Integer;
begin
  VariablesNode := FDoc.Root.Values['variables'];
  if VariablesNode = nil then
    Exit; // No variables section, list remains empty

  if not VariablesNode.IsSequence then
    raise Exception.Create('"variables" must be a sequence (array) in YAML.');

  VariableArray := VariablesNode.AsSequence;
  FVariables.Clear;
  for i := 0 to VariableArray.Count - 1 do
  begin
    VariableItem := VariableArray[i];

    VariableObj := TVariable.Create;
    VariableObj.Name := VariableItem.Values['name'].AsString;

    TypeStr := VariableItem.Values['type'].AsString;
    if SameText(TypeStr, 'string') then
      VariableObj.&Type := vtString
    else
      raise Exception.CreateFmt('Unknown variable type "%s" at index %d.', [TypeStr, i]);

    FVariables.Add(VariableObj);
  end;
end;

procedure TParserYAML.LoadPrompts;
var
  PromptsNode: IYAMLValue;
  PromptArray: IYAMLSequence;
  PromptItem: IYAMLValue;
  PromptObj: TPrompt;
  InputsNode: IYAMLValue;
  InputArray: IYAMLSequence;
  InputItem: IYAMLValue;
  PromptInput: TPromptInput;
  VariableName: string;
  FoundVariable: TVariable;
  TypeStr: string;
  OptionsNode: IYAMLValue;
  OptionsArray: IYAMLSequence;
  OptionItem: IYAMLValue;
  OptionObj: TPromptInputOption;
  i, j, k, l: Integer;
begin
  PromptsNode := FDoc.Root.Values['prompts'];
  if PromptsNode = nil then
    Exit; // No prompts section, list remains empty

  if not PromptsNode.IsSequence then
    raise Exception.Create('"prompts" must be a sequence (array) in YAML.');

  PromptArray := PromptsNode.AsSequence;
  for i := 0 to PromptArray.Count - 1 do
  begin
    PromptItem := PromptArray[i];

    PromptObj := TPrompt.Create;
    PromptObj.Name := PromptItem.Values['name'].AsString;
    PromptObj.&Result := PromptItem.Values['result'].AsString;

    // Load inputs
    InputsNode := PromptItem.Values['inputs'];
    if InputsNode <> nil then
    begin
      if not InputsNode.IsSequence then
        raise Exception.CreateFmt('"inputs" must be a sequence (array) for prompt at index %d.', [i]);

      InputArray := InputsNode.AsSequence;
      for j := 0 to InputArray.Count - 1 do
      begin
        InputItem := InputArray[j];
        
        PromptInput := TPromptInput.Create;
        PromptInput.Input := InputItem.Values['input'].AsString;

        // Find variable by name
        VariableName := InputItem.Values['variable'].AsString;
        FoundVariable := nil;
        for k := 0 to FVariables.Count - 1 do
        begin
          if SameText(FVariables[k].Name, VariableName) then
          begin
            FoundVariable := FVariables[k];
            Break;
          end;
        end;

        if FoundVariable = nil then
          raise Exception.CreateFmt('Variable "%s" not found for input at index %d in prompt at index %d.', [VariableName, j, i]);

        PromptInput.Variable := FoundVariable;

        // Convert type string to enum
        TypeStr := InputItem.Values['type'].AsString;
        if SameText(TypeStr, 'InputString') then
          PromptInput.&Type := ptInputString
        else if SameText(TypeStr, 'CheckList') then
          PromptInput.&Type := ptChecklist
        else if SameText(TypeStr, 'ArrayList') then
          PromptInput.&Type := ptArrayList
        else
          raise Exception.CreateFmt('Unknown prompt input type "%s" at index %d in prompt at index %d.', [TypeStr, j, i]);

        // Load options
        OptionsNode := InputItem.Values['options'];
        if OptionsNode is TYAMLSequence then
        begin
          try
            OptionsArray := OptionsNode.AsSequence;
            for l := 0 to OptionsArray.Count - 1 do
            begin
              OptionItem := OptionsArray[l];
                
              OptionObj := TPromptInputOption.Create;
              OptionObj.Name := OptionItem.Values['name'].AsString;
              OptionObj.Value := OptionItem.Values['value'].AsString;
              PromptInput.Options.Add(OptionObj);
            end;
          except
            on E: EInvalidCast do
              raise Exception.CreateFmt('"options" must be a sequence (array) for input at index %d in prompt at index %d.', [j, i]);
            on E: Exception do
              raise Exception.CreateFmt('Error loading options for input at index %d in prompt at index %d: %s', [j, i, E.Message]);
          end;
        end;

        PromptObj.Inputs.Add(PromptInput);
      end;
    end;

    FPrompts.Add(PromptObj);
  end;
end;

procedure TParserYAML.LoadFiles;
var
  FilesNode: IYAMLValue;
  FileArray: IYAMLSequence;
  FileItem: IYAMLValue;
  FileObj: TFileData;
  PromptName: string;
  PromptNode: IYAMLValue;
  FoundPrompt: TPrompt;
  i, j: Integer;
begin
  FilesNode := FDoc.Root.Values['files'];
  if FilesNode = nil then
    Exit; // No files section, list remains empty

  if not FilesNode.IsSequence then
    raise Exception.Create('"files" must be a sequence (array) in YAML.');

  FileArray := FilesNode.AsSequence;
  for i := 0 to FileArray.Count - 1 do
  begin
    FileItem := FileArray[i];

    FileObj := TFileData.Create(
      FileItem.Values['path'].AsString,
      FileItem.Values['content'].AsString
    );

    // Check if prompt is specified in YAML
    PromptNode := FileItem.Values['prompt'];
    if PromptNode <> nil then
    begin
      PromptName := PromptNode.AsString;
      FoundPrompt := nil;

      // Search for the prompt in the prompts list
      for j := 0 to FPrompts.Count - 1 do
      begin
        if SameText(FPrompts[j].Name, PromptName) then
        begin
          FoundPrompt := FPrompts[j];
          Break;
        end;
      end;

      if FoundPrompt = nil then
        raise Exception.CreateFmt('Prompt "%s" not found for file at index %d.', [PromptName, i]);

      FileObj.Prompt := FoundPrompt;
    end;

    FFiles.Add(FileObj);
  end;
end;

procedure TParserYAML.BuildAll;
var
  FileItem: TFileData;
begin
  for FileItem in FFiles do
  begin
    FFileBuilder.Build(FileItem);
    Writeln(Format('Created %s', [FileItem.Path]));
  end;
end;

procedure TParserYAML.ValidateVersion;
begin
  if not MatchText(FVersion, SUPPORTED_VERSIONS) then
    raise EUnsupportedTemplateVersion.CreateFmt(
      'Template version not supported: %s. Supported versions: %s',
      [FVersion, string.Join(', ', SUPPORTED_VERSIONS)]
    );
end;


end.
