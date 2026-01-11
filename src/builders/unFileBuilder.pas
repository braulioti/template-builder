unit unFileBuilder;

interface

uses
  System.IOUtils, System.SysUtils, System.Classes, unFileType, unPromptBuilder;

type
  TFileBuilder = class
  private
    FPromptBuilder: TPromptBuilder;
  public
    constructor Create;
    destructor Destroy; override;
    procedure Build(const AFile: TFileData);
  end;

implementation

{ TFileBuilder }

constructor TFileBuilder.Create;
begin
  inherited Create;
  FPromptBuilder := TPromptBuilder.Create;
end;

destructor TFileBuilder.Destroy;
begin
  FPromptBuilder.Free;
  inherited;
end;

procedure TFileBuilder.Build(const AFile: TFileData);
var
  FullPath: string;
  Directory: string;
  FileContent: TStringList;
begin
  if AFile = nil then
    Exit;

  if AFile.Path.IsEmpty then
    raise Exception.Create('File path cannot be empty.');

  FullPath := TPath.Combine(GetCurrentDir, AFile.Path);
  Directory := TPath.GetDirectoryName(FullPath);

  if not Directory.IsEmpty and not TDirectory.Exists(Directory) then
    TDirectory.CreateDirectory(Directory);

  FileContent := TStringList.Create;
  try
    if AFile.Prompt = nil then
      FileContent.Text := FPromptBuilder.GetContent(AFile.Content, AFile.Variables)
    else
      FileContent.Text := FPromptBuilder.Build(AFile.Prompt, AFile.Variables);

    FileContent.SaveToFile(FullPath, TEncoding.UTF8);
  finally
    FileContent.Free;
  end;
end;

end.
