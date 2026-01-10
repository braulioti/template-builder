unit unVariableType;

interface

type
  TVariableType = (vtString);

type
  TVariable = class
  private
    FName: string;
    FVariableType: TVariableType;
    FValue: Variant;
  public
    property Name: string read FName write FName;
    property &Type: TVariableType read FVariableType write FVariableType;
    property Value: Variant read FValue write FValue;
  end;

implementation

end.
