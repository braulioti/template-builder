unit unRemoteFileType;

interface

type
  TRemoteFile = class
  private
    FUri: string;
    FPath: string;
  public
    constructor Create(const AUri, APath: string);
    property Uri: string read FUri write FUri;
    property Path: string read FPath write FPath;
  end;

implementation

{ TRemoteFile }

constructor TRemoteFile.Create(const AUri, APath: string);
begin
  inherited Create;
  FUri := AUri;
  FPath := APath;
end;

end.
