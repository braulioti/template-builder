unit unPromptType;

interface

uses
  System.Generics.Collections, unVariableType;

type
  TPromptType = (ptInputString, ptChecklist, ptArrayList);

type
  TPromptInputOption = class
  private
    FName: string;
    FValue: string;
  public
    property Name: string read FName write FName;
    property Value: string read FValue write FValue;
  end;

type
  TPromptInput = class
  private
    FVariable: TVariable;
    FInput: string;
    FPromptType: TPromptType;
    FOptions: TObjectList<TPromptInputOption>;
  public
    constructor Create;
    property Variable: TVariable read FVariable write FVariable;
    property Input: string read FInput write FInput;
    property &Type: TPromptType read FPromptType write FPromptType;
    property Options: TObjectList<TPromptInputOption> read FOptions;
  end;

type
  TPrompt = class
  private
    FName: string;
    FResult: string;
    FInputs: TObjectList<TPromptInput>;
  public
    constructor Create;
    property Name: string read FName write FName;
    property &Result: string read FResult write FResult;
    property Inputs: TObjectList<TPromptInput> read FInputs;
  end;

implementation

{ TPromptInputOption }

{ TPromptInput }

constructor TPromptInput.Create;
begin
  inherited Create;
  FOptions := TObjectList<TPromptInputOption>.Create(True);
end;


{ TPrompt }

constructor TPrompt.Create;
begin
  inherited Create;
  FInputs := TObjectList<TPromptInput>.Create(True);
end;


end.
