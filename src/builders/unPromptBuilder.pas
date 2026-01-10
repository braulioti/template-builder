unit unPromptBuilder;

interface

uses
  System.SysUtils, System.Variants, System.Generics.Collections, System.Classes,
  System.StrUtils, Winapi.Windows, unPromptType, System.RegularExpressions;

type
  TPromptBuilder = class
  private
    procedure GetInputString(const APromptInput: TPromptInput);
    procedure GetChecklist(const APromptInput: TPromptInput);
    procedure GetArrayList(const APromptInput: TPromptInput);
    function GetContent(const APrompt: TPrompt): string;
  public
    function Build(const APrompt: TPrompt): string;
  end;

implementation

function ReadConsoleKey(var Key: Word; var KeyPressed: Boolean): Boolean;
var
  InputRecord: TInputRecord;
  NumRead: DWORD;
  StdIn: THandle;
begin
  Result := False;
  KeyPressed := False;
  StdIn := GetStdHandle(STD_INPUT_HANDLE);
  if StdIn = INVALID_HANDLE_VALUE then
    Exit;

  // Check if there's input available
  if not PeekConsoleInput(StdIn, InputRecord, 1, NumRead) then
    Exit;

  if NumRead = 0 then
    Exit;

  // Read the input
  if ReadConsoleInput(StdIn, InputRecord, 1, NumRead) then
  begin
    if InputRecord.EventType = KEY_EVENT then
    begin
      if InputRecord.Event.KeyEvent.bKeyDown then
      begin
        Key := InputRecord.Event.KeyEvent.wVirtualKeyCode;
        KeyPressed := True;
        Result := True;
      end;
    end;
  end;
end;

{ TPromptBuilder }

procedure TPromptBuilder.GetInputString(const APromptInput: TPromptInput);
var
  UserInput: string;
begin
  if APromptInput = nil then
    Exit;

  if APromptInput.Variable = nil then
    raise Exception.Create('Variable is nil in TPromptInput.');

  Write(APromptInput.Input);
  Readln(UserInput);
  APromptInput.Variable.Value := UserInput;
end;

procedure TPromptBuilder.GetChecklist(const APromptInput: TPromptInput);
var
  Options: TList<TPromptInputOption>;
  Selected: TList<Boolean>;
  CurrentIndex: Integer;
  Key: Word;
  KeyPressed: Boolean;
  Done: Boolean;
  i: Integer;
  SelectedValues: string;
begin
  if APromptInput = nil then
    Exit;

  if APromptInput.Variable = nil then
    raise Exception.Create('Variable is nil in TPromptInput.');

  if APromptInput.Options.Count = 0 then
    raise Exception.Create('No options available for checklist input.');

  Options := TList<TPromptInputOption>.Create;
  Selected := TList<Boolean>.Create;
  try
    // Initialize options and selection state
    for i := 0 to APromptInput.Options.Count - 1 do
    begin
      Options.Add(APromptInput.Options[i]);
      Selected.Add(False);
    end;

    CurrentIndex := 0;
    Done := False;

    // Display prompt
    Writeln;
    Writeln(APromptInput.Input);
    Writeln;

    // Main loop for navigation and selection
    repeat
      // Redraw options
      for i := 0 to Options.Count - 1 do
      begin
        if i = CurrentIndex then
          Write('> ')
        else
          Write('  ');

        if Selected[i] then
          Write('[ X ] ')
        else
          Write('[   ] ');

        Writeln(Options[i].Name);
      end;

      Writeln('Use Up/Down arrows to navigate, Space to select/deselect, Enter to confirm');

      // Read key input
      repeat
        if ReadConsoleKey(Key, KeyPressed) then
        begin
          // Clear all displayed lines: move up and clear each line
          // We're currently at the line AFTER the instruction, so we go up Options.Count + 1 times
          for i := 1 to Options.Count + 1 do
          begin
            Write(#27'[A'); // Move cursor up one line
            Write(#27'[2K'); // Clear the entire line we just moved to
          end;

          // Process key
          case Key of
            VK_UP:
              if CurrentIndex > 0 then
                Dec(CurrentIndex);
            VK_DOWN:
              if CurrentIndex < Options.Count - 1 then
                Inc(CurrentIndex);
            Ord(' '):
              Selected[CurrentIndex] := not Selected[CurrentIndex];
            VK_RETURN:
              Done := True;
          end;
          Break;
        end;
        Sleep(10);
      until False;

    until Done;

    // Clear the instruction line one last time
    Write(#27'[A'); // Move cursor up (from instruction line)
    Write(#27'[2K'); // Clear entire line

    // Build result string with selected values
    SelectedValues := '';
    for i := 0 to Options.Count - 1 do
    begin
      if Selected[i] then
      begin
        if SelectedValues <> '' then
          SelectedValues := SelectedValues + #13#10;
        SelectedValues := SelectedValues + Options[i].Value;
      end;
    end;

    Writeln;
    // Store result in variable
    APromptInput.Variable.Value := SelectedValues;
  finally
    Selected.Free;
    Options.Free;
  end;
end;

procedure TPromptBuilder.GetArrayList(const APromptInput: TPromptInput);
var
  UserInput: string;
  Lines: TStringList;
begin
  if APromptInput = nil then
    Exit;

  if APromptInput.Variable = nil then
    raise Exception.Create('Variable is nil in TPromptInput.');

  Lines := TStringList.Create;
  try
    // Display prompt
    Writeln;
    Writeln(APromptInput.Input);
    Writeln('Enter each option and press Enter. Leave empty and press Enter to finish:');
    Writeln;

    // Read multiple lines until empty line
    repeat
      Write('> ');
      Readln(UserInput);
      
      // If line is not empty, add to list
      if not UserInput.Trim.IsEmpty then
        Lines.Add(UserInput);
    until UserInput.Trim.IsEmpty;

    // Store result as multiple lines separated by line breaks
    APromptInput.Variable.Value := Lines.Text;
    Writeln;
  finally
    Lines.Free;
  end;
end;

function TPromptBuilder.GetContent(const APrompt: TPrompt): string;
var
  PromptInput: TPromptInput;
  VariableName: string;
  VariableValue: string;
  Placeholder: string;
  PrefixPattern: string;
  Lines: TStringList;
  ProcessedValue: string;
  i, j: Integer;
  Match: TMatch;
  Matches: TMatchCollection;
  Prefix: string;
  VarName: string;
begin
  if APrompt = nil then
  begin
    Result := '';
    Exit;
  end;

  Result := APrompt.&Result;

  // First, process special pattern {{"prefix" | variableName}}
  // Pattern: {{"..." | varName}} or {{'...' | varName}}
  PrefixPattern := '\{\{"([^"]+)"\s*\|\s*(\w+)\}\}';
  Matches := TRegEx.Matches(Result, PrefixPattern);
  
  // Process matches from end to start to maintain positions
  for i := Matches.Count - 1 downto 0 do
  begin
    Match := Matches[i];
    Prefix := Match.Groups[1].Value;
    VarName := Match.Groups[2].Value;
    
    // Find the variable value
    VariableValue := '';
    for PromptInput in APrompt.Inputs do
    begin
      if (PromptInput.Variable <> nil) and SameText(PromptInput.Variable.Name, VarName) then
      begin
        if not VarIsNull(PromptInput.Variable.Value) and not VarIsEmpty(PromptInput.Variable.Value) then
          VariableValue := VarToStr(PromptInput.Variable.Value);
        Break;
      end;
    end;
    
    // Process the value: split by lines and add prefix to each
    Lines := TStringList.Create;
    try
      Lines.Text := VariableValue;
      ProcessedValue := '';
      for j := 0 to Lines.Count - 1 do
      begin
        if not Lines[j].Trim.IsEmpty then
        begin
          if ProcessedValue <> '' then
            ProcessedValue := ProcessedValue + #13#10;
          ProcessedValue := ProcessedValue + Prefix + Lines[j];
        end;
      end;
      
      // Replace the matched pattern with processed value
      // Match.Index is 1-based, Delete and Insert use 1-based too in Delphi
      // So we use the index directly
      Delete(Result, Match.Index, Match.Length);
      Insert(ProcessedValue, Result, Match.Index);
    finally
      Lines.Free;
    end;
  end;

  // Then, replace normal placeholders {{variableName}} with variable values
  for PromptInput in APrompt.Inputs do
  begin
    if PromptInput.Variable <> nil then
    begin
      VariableName := PromptInput.Variable.Name;
      if not VarIsNull(PromptInput.Variable.Value) and not VarIsEmpty(PromptInput.Variable.Value) then
        VariableValue := VarToStr(PromptInput.Variable.Value)
      else
        VariableValue := '';

      Placeholder := '{{' + VariableName + '}}';
      Result := StringReplace(Result, Placeholder, VariableValue, [rfReplaceAll]);
    end;
  end;
end;

function TPromptBuilder.Build(const APrompt: TPrompt): string;
var
  PromptInput: TPromptInput;
begin
  if APrompt = nil then
  begin
    Result := '';
    Exit;
  end;

  // Iterate over all inputs
  for PromptInput in APrompt.Inputs do
  begin
    if PromptInput.&Type = ptInputString then
    begin
      GetInputString(PromptInput);
    end
    else if PromptInput.&Type = ptChecklist then
    begin
      GetChecklist(PromptInput);
    end
    else if PromptInput.&Type = ptArrayList then
    begin
      GetArrayList(PromptInput);
    end;
  end;

  Result := GetContent(APrompt);
end;

end.
