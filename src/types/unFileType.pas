unit unFileType;

interface

uses
  unPromptType;

type
  TFileData = class
  private
    FPath: string;
    FContent: string;
    FPrompt: TPrompt;
  public
    constructor Create(const APath, AContent: string);
    property Path: string read FPath write FPath;
    property Content: string read FContent write FContent;
    property Prompt: TPrompt read FPrompt write FPrompt;
  end;

implementation

{ TFile }

constructor TFileData.Create(const APath, AContent: string);
begin
  inherited Create;
  FPath := APath;
  FContent := AContent;
end;

end.
