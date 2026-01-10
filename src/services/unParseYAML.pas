unit unParseYAML;

interface

uses
  System.SysUtils, System.Generics.Collections, VSoft.YAML, StrUtils,
  unFileType, unBuildFiles;

type
  EUnsupportedTemplateVersion = class(Exception);

type TParserYAML = class
private
  FVersion: string;
  FDoc: IYAMLDocument;
  FFiles: TObjectList<TFileData>;
  FFileBuilder: TBuildFile;
  procedure ValidateVersion;
  procedure LoadFiles;
  procedure Initializer;
public
  constructor Create(const FileName: string);
  destructor Destroy; override;
  procedure BuildAll;
  property Version: string read FVersion;
  property Document: IYAMLDocument read FDoc;
  property Files: TObjectList<TFileData> read FFiles;
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
  LoadFiles;
end;

destructor TParserYAML.Destroy;
begin
  FFileBuilder.Free;
  FFiles.Free;
  inherited;
end;

procedure TParserYAML.Initializer;
begin
  FFiles := TObjectList<TFileData>.Create(True);
  FFileBuilder := TBuildFile.Create;
end;

procedure TParserYAML.LoadFiles;
var
  FilesNode: IYAMLValue;
  FileArray: IYAMLSequence;
  FileItem: IYAMLValue;
  FileObj: TFileData;
  i: Integer;
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
