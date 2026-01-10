unit unBuildFiles;

interface

uses
  System.IOUtils, System.SysUtils, System.Classes, unFileType;

type
  TBuildFile = class
  public
    procedure Build(const AFile: TFileData);
  end;

implementation

{ TBuildFile }

procedure TBuildFile.Build(const AFile: TFileData);
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
    FileContent.Text := AFile.Content;
    FileContent.SaveToFile(FullPath, TEncoding.UTF8);
  finally
    FileContent.Free;
  end;
end;

end.
