unit unRemoteFileBuilder;

interface

uses
  System.IOUtils, System.SysUtils, System.Classes, System.Net.HttpClient,
  unRemoteFileType;

type
  TRemoteFileBuilder = class
  public
    procedure Build(const ARemoteFile: TRemoteFile);
  end;

implementation

{ TRemoteFileBuilder }

procedure TRemoteFileBuilder.Build(const ARemoteFile: TRemoteFile);
var
  FullPath: string;
  Directory: string;
  HTTPClient: THTTPClient;
  Response: IHTTPResponse;
  Stream: TFileStream;
begin
  if ARemoteFile = nil then
    Exit;

  if ARemoteFile.Uri.IsEmpty then
    raise Exception.Create('Remote file URI cannot be empty.');

  if ARemoteFile.Path.IsEmpty then
    raise Exception.Create('Remote file path cannot be empty.');

  FullPath := TPath.Combine(GetCurrentDir, ARemoteFile.Path);
  Directory := TPath.GetDirectoryName(FullPath);

  // Create directory structure if it doesn't exist
  if not Directory.IsEmpty and not TDirectory.Exists(Directory) then
    TDirectory.CreateDirectory(Directory);

  // Show download status
  Write(Format('Downloading... %s', [ARemoteFile.Path]));

  // Download the remote file
  HTTPClient := THTTPClient.Create;
  try
    Response := HTTPClient.Get(ARemoteFile.Uri);
    
    if Response.StatusCode <> 200 then
      raise Exception.CreateFmt('Failed to download remote file. HTTP Status: %d', [Response.StatusCode]);

    // Save the downloaded content to file
    Stream := TFileStream.Create(FullPath, fmCreate);
    try
      Response.ContentStream.Position := 0;
      Stream.CopyFrom(Response.ContentStream, Response.ContentStream.Size);
    finally
      Stream.Free;
    end;

    // Show success status
    Writeln(' <OK>');
  finally
    HTTPClient.Free;
  end;
end;

end.
