unit unPromptBuilder;

interface

uses
  System.SysUtils, System.Variants, System.Generics.Collections, System.Classes,
  System.StrUtils, Winapi.Windows, unPromptType, unVariableType, System.RegularExpressions;

type
  TPromptBuilder = class
  private
    procedure GetInputString(const APromptInput: TPromptInput);
    procedure GetChecklist(const APromptInput: TPromptInput);
    procedure GetArrayList(const APromptInput: TPromptInput);
    function ResolveVariableValue(const AVariables: TObjectList<TVariable>; const AVariableName: string): string;
    function ParseFunctionExpression(const AVariables: TObjectList<TVariable>; const AExpression: string): string;
    function ExecuteFunction(const AFunctionName: string; const AArguments: TArray<string>): string;
  public
    function GetContent(const AContent: string; const AVariables: TObjectList<TVariable>): string;
    function Build(const APrompt: TPrompt; const AVariables: TObjectList<TVariable>): string;
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

function TPromptBuilder.GetContent(const AContent: string; const AVariables: TObjectList<TVariable>): string;
var
  Variable: TVariable;
  VariableName: string;
  VariableValue: string;
  Placeholder: string;
  PrefixPattern: string;
  Lines: TStringList;
  ProcessedValue: string;
  i, j, k, l: Integer;
  Match: TMatch;
  Matches: TMatchCollection;
  Prefix: string;
  VarName: string;
  FunctionPattern: string;
  FunctionMatches: TMatchCollection;
  FunctionMatch: TMatch;
  FunctionExpression: string;
  FunctionResult: string;
  Processed: Boolean;
  MaxIterations: Integer;
begin
  if AVariables = nil then
  begin
    Result := AContent;
    Exit;
  end;

  Result := AContent;

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
    VariableValue := ResolveVariableValue(AVariables, VarName);
    
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
      Delete(Result, Match.Index, Match.Length);
      Insert(ProcessedValue, Result, Match.Index);
    finally
      Lines.Free;
    end;
  end;

  // Second, process function expressions {{upper(variable)}}, {{lower(variable)}}, {{replace(" ", "_", variable)}}, etc.
  // Pattern: {{functionName(arg1, arg2, ...)}}
  // Process recursively - ParseFunctionExpression handles nested functions internally
  MaxIterations := 100; // Safety limit
  l := 0;
  
  repeat
    Processed := False;
    i := 1;
    // Find function expressions manually to properly handle nested functions
    while i <= Length(Result) - 2 do
    begin
      // Look for {{ that starts a function expression
      if (Result[i] = '{') and (Result[i + 1] = '{') then
      begin
        // Found {{, now find the matching }}
        var StartPos: Integer;
        var FuncStart: Integer;
        var ParenDepth: Integer;
        var InQuotes: Boolean;
        var QuoteChar: Char;
        var FoundFunc: Boolean;
        
        StartPos := i;
        FuncStart := 0;
        FoundFunc := False;
        ParenDepth := 0;
        InQuotes := False;
        QuoteChar := #0;
        j := i + 2; // Skip {{
        
        // Look for function name and opening parenthesis
        while j <= Length(Result) - 1 do
        begin
          if not InQuotes then
          begin
            if (Result[j] = '"') or (Result[j] = '''') then
            begin
              InQuotes := True;
              QuoteChar := Result[j];
            end
            else if Result[j] = '(' then
            begin
              if ParenDepth = 0 then
              begin
                FuncStart := i + 2; // Start of function expression (after {{)
                FoundFunc := True;
              end;
              Inc(ParenDepth);
            end
            else if Result[j] = ')' then
            begin
              Dec(ParenDepth);
              if (ParenDepth = 0) and FoundFunc then
              begin
                // Found closing paren, check for }}
                if (j < Length(Result) - 1) and (Result[j + 1] = '}') and (Result[j + 2] = '}') then
                begin
                  // Found complete function expression {{func(...)}}
                  // Include the closing ')' in the expression
                  FunctionExpression := Copy(Result, FuncStart, j - FuncStart + 1);
                  FunctionResult := ParseFunctionExpression(AVariables, FunctionExpression);
                  
                  // Replace {{...}} with result
                  Delete(Result, StartPos, j + 3 - StartPos);
                  Insert(FunctionResult, Result, StartPos);
                  
                  Processed := True;
                  Break; // Exit inner while loop to restart from beginning
                end;
              end;
            end
            else if (Result[j] = '}') and (Result[j + 1] = '}') and not FoundFunc then
            begin
              // Not a function, skip this {{ and continue searching
              Break; // Exit inner while loop, continue outer while loop
            end;
          end
          else
          begin
            if Result[j] = QuoteChar then
            begin
              // Check if escaped
              if (j < Length(Result)) and (Result[j + 1] = QuoteChar) then
                Inc(j) // Skip escaped quote
              else
                InQuotes := False;
            end;
          end;
          Inc(j);
        end;
        
        // If we processed a function, break from outer loop to restart
        if Processed then
          Break; // Exit outer while loop to restart from beginning
      end;
      Inc(i);
    end;
    
    Inc(l);
  until not Processed or (l >= MaxIterations);

  // Finally, replace normal placeholders {{variableName}} with variable values
  for Variable in AVariables do
  begin
    if Variable <> nil then
    begin
      VariableName := Variable.Name;
      if not VarIsNull(Variable.Value) and not VarIsEmpty(Variable.Value) then
        VariableValue := VarToStr(Variable.Value)
      else
        VariableValue := '';

      Placeholder := '{{' + VariableName + '}}';
      Result := StringReplace(Result, Placeholder, VariableValue, [rfReplaceAll]);
    end;
  end;
end;


function TPromptBuilder.ResolveVariableValue(const AVariables: TObjectList<TVariable>; const AVariableName: string): string;
var
  Variable: TVariable;
begin
  Result := '';
  if AVariables = nil then
    Exit;

  for Variable in AVariables do
  begin
    if (Variable <> nil) and SameText(Variable.Name, AVariableName) then
    begin
      if not VarIsNull(Variable.Value) and not VarIsEmpty(Variable.Value) then
        Result := VarToStr(Variable.Value);
      Exit;
    end;
  end;
end;

function TPromptBuilder.ExecuteFunction(const AFunctionName: string; const AArguments: TArray<string>): string;
var
  ArgCount: Integer;
begin
  Result := '';
  ArgCount := Length(AArguments);

  if SameText(AFunctionName, 'upper') then
  begin
    if ArgCount <> 1 then
      raise Exception.CreateFmt('Function "upper" expects 1 argument, got %d', [ArgCount]);
    Result := UpperCase(AArguments[0]);
  end
  else if SameText(AFunctionName, 'lower') then
  begin
    if ArgCount <> 1 then
      raise Exception.CreateFmt('Function "lower" expects 1 argument, got %d', [ArgCount]);
    Result := LowerCase(AArguments[0]);
  end
  else if SameText(AFunctionName, 'replace') then
  begin
    if ArgCount <> 3 then
      raise Exception.CreateFmt('Function "replace" expects 3 arguments, got %d', [ArgCount]);
    Result := StringReplace(AArguments[2], AArguments[0], AArguments[1], [rfReplaceAll]);
  end
  else
    raise Exception.CreateFmt('Unknown function: %s', [AFunctionName]);
end;

function TPromptBuilder.ParseFunctionExpression(const AVariables: TObjectList<TVariable>; const AExpression: string): string;
var
  FunctionName: string;
  ArgsString: string;
  Arguments: TArray<string>;
  i: Integer;
  InQuotes: Boolean;
  QuoteChar: Char;
  FunctionDepth: Integer;
  StartPos: Integer;
  j: Integer;
  CurrentChar: Char;
  ParsedArguments: TList<string>;
  
  function ParseArgument(const ArgStr: string): string;
  var
    Trimmed: string;
  begin
    Trimmed := Trim(ArgStr);
    
    // Check if it's a string literal (starts and ends with quotes)
    if (Length(Trimmed) >= 2) and 
       ((Trimmed[1] = '"') and (Trimmed[Length(Trimmed)] = '"')) then
    begin
      // Remove quotes and unescape
      Result := Copy(Trimmed, 2, Length(Trimmed) - 2);
      Result := StringReplace(Result, '""', '"', [rfReplaceAll]);
    end
    else if (Length(Trimmed) >= 2) and 
            ((Trimmed[1] = '''') and (Trimmed[Length(Trimmed)] = '''')) then
    begin
      // Remove single quotes
      Result := Copy(Trimmed, 2, Length(Trimmed) - 2);
      Result := StringReplace(Result, '''''', '''', [rfReplaceAll]);
    end
    else
    begin
      // Check if it's a nested function call
      if Pos('(', Trimmed) > 0 then
      begin
        // It's a nested function - parse it recursively
        // Check if the function expression has balanced parentheses
        var FuncExpr: string;
        var OpenParenPos: Integer;
        var Depth: Integer;
        var k: Integer;

        FuncExpr := Trimmed;
        OpenParenPos := Pos('(', FuncExpr);
        
        if OpenParenPos > 0 then
        begin
          Depth := 0;
          // Count parentheses to see if we have a matching closing ')'
          for k := OpenParenPos to Length(FuncExpr) do
          begin
            if FuncExpr[k] = '(' then
              Inc(Depth)
            else if FuncExpr[k] = ')' then
            begin
              Dec(Depth);
              if Depth = 0 then
                Break; // Found matching ')'
            end;
          end;
          
          // If Depth > 0, we didn't find a matching ')', so we need to add one
          if Depth > 0 then
            FuncExpr := FuncExpr + ')';
        end;
        
        Result := ParseFunctionExpression(AVariables, FuncExpr);
      end
      else
      begin
        // It's a variable name
        Result := ResolveVariableValue(AVariables, Trimmed);
      end;
    end;
  end;
  
begin
  Result := '';
  
  // Find function name and arguments
  i := Pos('(', AExpression);
  if i = 0 then
    raise Exception.CreateFmt('Invalid function expression: %s', [AExpression]);
    
  FunctionName := Trim(Copy(AExpression, 1, i - 1));
  // Get everything after '(' - use MaxInt to get all remaining characters
  ArgsString := Copy(AExpression, i + 1, MaxInt);
  // Remove all trailing ')' characters (there should be only one, but handle multiple just in case)
  while (Length(ArgsString) > 0) and (ArgsString[Length(ArgsString)] = ')') do
    ArgsString := Copy(ArgsString, 1, Length(ArgsString) - 1);
  
  // Parse arguments
  ParsedArguments := TList<string>.Create;
  try
    if Trim(ArgsString) <> '' then
    begin
      StartPos := 1;
      InQuotes := False;
      QuoteChar := #0;
      FunctionDepth := 0;
      j := 1;
      
      while j <= Length(ArgsString) do
      begin
        CurrentChar := ArgsString[j];
        
        if not InQuotes then
        begin
          if (CurrentChar = '"') or (CurrentChar = '''') then
          begin
            InQuotes := True;
            QuoteChar := CurrentChar;
          end
          else if CurrentChar = '(' then
          begin
            Inc(FunctionDepth);
          end
          else if CurrentChar = ')' then
          begin
            Dec(FunctionDepth);
          end
          else if (CurrentChar = ',') and (FunctionDepth = 0) then
          begin
            // Found an argument separator
            var Arg: string;
            Arg := Copy(ArgsString, StartPos, j - StartPos);
            Arg := Trim(Arg);
            if Arg <> '' then
              ParsedArguments.Add(Arg);
            StartPos := j + 1;
          end;
        end
        else
        begin
          if CurrentChar = QuoteChar then
          begin
            // Check if it's an escaped quote
            if (j < Length(ArgsString)) and (ArgsString[j + 1] = QuoteChar) then
              Inc(j) // Skip escaped quote
            else
              InQuotes := False;
          end;
        end;
        
        Inc(j);
      end;
      
      // Add the last argument (or the only argument if there are no commas)
      if StartPos <= Length(ArgsString) then
      begin
        var LastArg: string;
        LastArg := Copy(ArgsString, StartPos, MaxInt);
        LastArg := Trim(LastArg);
        if LastArg <> '' then
          ParsedArguments.Add(LastArg);
      end;
    end;
    
    // Resolve arguments
    SetLength(Arguments, ParsedArguments.Count);
    for var ArgIndex := 0 to ParsedArguments.Count - 1 do
    begin
      Arguments[ArgIndex] := ParseArgument(ParsedArguments[ArgIndex]);
    end;
  finally
    ParsedArguments.Free;
  end;
  
  // Execute function
  Result := ExecuteFunction(FunctionName, Arguments);
end;

function TPromptBuilder.Build(const APrompt: TPrompt; const AVariables: TObjectList<TVariable>): string;
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

  Result := GetContent(APrompt.Result, AVariables);
end;

end.
