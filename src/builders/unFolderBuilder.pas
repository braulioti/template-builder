unit unFolderBuilder;

interface

uses
  System.IOUtils, System.SysUtils, unFileType;

type
  TFolderBuilder = class
  public
    procedure Build(const AFolder: TFileData);
  end;

implementation

{ TFolderBuilder }

procedure TFolderBuilder.Build(const AFolder: TFileData);
var
  FullPath: string;
  Directory: string;
begin
  if AFolder = nil then
    Exit;

  if AFolder.Path.IsEmpty then
    raise Exception.Create('Folder path cannot be empty.');

  FullPath := TPath.Combine(GetCurrentDir, AFolder.Path);

  // Check if Path ends with a directory separator (it's a directory path)
  if AFolder.Path.EndsWith(TPath.DirectorySeparatorChar) or 
     AFolder.Path.EndsWith(TPath.AltDirectorySeparatorChar) then
  begin
    // It's explicitly a directory path, use FullPath directly
    Directory := FullPath;
  end
  else
  begin
    // Get the directory part of the path (same logic as TFileBuilder)
    Directory := TPath.GetDirectoryName(FullPath);
  end;

  // Create the directory structure (only creates if it doesn't exist)
  if not Directory.IsEmpty and not TDirectory.Exists(Directory) then
    TDirectory.CreateDirectory(Directory);
end;

end.
