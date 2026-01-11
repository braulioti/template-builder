program TemplateBuilder;

{$APPTYPE CONSOLE}

{$R *.res}

uses
  System.SysUtils,
  System.IOUtils,
  unFileBuilder in 'builders\unFileBuilder.pas',
  unParseYAML in 'services\unParseYAML.pas',
  unFileType in 'types\unFileType.pas',
  unVariableType in 'types\unVariableType.pas',
  unPromptType in 'types\unPromptType.pas',
  unPromptBuilder in 'builders\unPromptBuilder.pas',
  unFolderBuilder in 'builders\unFolderBuilder.pas';

var
  YAMLParser: TParserYAML;

procedure ShowUsage;
begin
  Writeln('Use: templatebuilder.exe <arquivo.yaml>');
end;

begin
  try

    Writeln;
    Writeln('***************************************************');
    Writeln('* TEMPLATE BUILDER - VERSION 1.0                  *');
    Writeln('* Generate project templates using YAML files     *');
    Writeln('* ----------------------------------------------- *');
    Writeln('* Author: Bráulio Figueiredo                      *');
    Writeln('* E-mail: braulio@braulioti.com.br                *');
    Writeln('* Website: https://brau.io                        *');
    Writeln('***************************************************');
    Writeln;

    if ParamCount < 1 then
    begin
      ShowUsage;
      Exit;
    end;

    if not System.IOUtils.TFile.Exists(ParamStr(1)) then
      raise Exception.Create('File not found: ' + ParamStr(1));

    YAMLParser := TParserYAML.Create(ParamStr(1));
    try
      YAMLParser.BuildAll;
    finally
      YAMLParser.Free;
    end;

    Writeln;
    Writeln('Template successfully generated.');
    Writeln('Thanks for using Template Builder!  :)');
    Writeln;
  except
    on E: Exception do
      Writeln(E.ClassName, ': ', E.Message);
  end;
end.
