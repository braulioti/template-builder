unit unParseYAML;

interface

uses
  System.SysUtils, VSoft.YAML, StrUtils;

type
  EUnsupportedTemplateVersion = class(Exception);

type TParserYAML = class
private
  FVersion: string;
  FDoc: IYAMLDocument;
  procedure ValidateVersion;
public
  constructor Create(const FileName: string);
  property Version: string read FVersion;
  property Document: IYAMLDocument read FDoc;
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

  // Validate version
  ValidateVersion;
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
