program TemplateBuilder;

{$APPTYPE CONSOLE}

{$R *.res}

uses
  System.SysUtils,
  System.IOUtils,
  unBuildFiles in 'builders\unBuildFiles.pas',
  unParseYAML in 'services\unParseYAML.pas';

var
  YAMLParser: TParserYAML;

procedure ShowUsage;
begin
  Writeln('Use: templatebuilder.exe <arquivo.yaml>');
end;

begin
  try
    if ParamCount < 1 then
    begin
      ShowUsage;
      Exit;
    end;

    if not TFile.Exists(ParamStr(1)) then
      raise Exception.Create('File not found: ' + ParamStr(1));

    YAMLParser := TParserYAML.Create(ParamStr(1));
    try
    finally
      YAMLParser.Free;
    end;

    Writeln('Template successfully generated.');
  except
    on E: Exception do
      Writeln(E.ClassName, ': ', E.Message);
  end;
end.
