[Setup]
AppName=WBTP
AppVersion=0.0.0
DefaultDirName={autopf}\WBTP
DefaultGroupName=WBTP
OutputDir=.
OutputBaseFilename=wbtp-cli-setup
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64compatible
ChangesEnvironment=yes
UninstallDisplayName=WBTP

[Files]
Source: "..\..\package\*"; DestDir: "{app}"; Flags: recursesubdirs

[Tasks]
Name: "addtopath"; Description: "Add WBTP to PATH (allows usage from any directory)"; Flags: unchecked

[Registry]
Root: HKCU; Subkey: "Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{code:AddToPath}"; Check: WizardIsTaskSelected("addtopath"); Flags: uninsdeletevalue

[Code]
function AddToPath(Param: String): String;
begin
	Result := ExpandConstant("{app}");
end;
