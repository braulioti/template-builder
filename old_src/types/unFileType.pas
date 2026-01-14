unit unFileType;

interface

uses
  System.Generics.Collections, unPromptType, unVariableType;

type
  TFileData = class
  private
    FPath: string;
    FContent: string;
    FPrompt: TPrompt;
    FVariables: TObjectList<TVariable>;
  public
    constructor Create(const APath, AContent: string);
    property Path: string read FPath write FPath;
    property Content: string read FContent write FContent;
    property Prompt: TPrompt read FPrompt write FPrompt;
    property Variables: TObjectList<TVariable> read FVariables write FVariables;
  end;

implementation

{ TFileData }

constructor TFileData.Create(const APath, AContent: string);
begin
  inherited Create;
  FPath := APath;
  FContent := AContent;
end;


end.
