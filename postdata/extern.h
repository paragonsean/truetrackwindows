#ifdef _MAIN_
TCHAR PostDataIniFileName[]      = TEXT( "postdata.ini" );
TCHAR ConfigSection[]            = TEXT( "Config" );
TCHAR RootSymphonyDirectoryKey[] = TEXT( "RootSymphonyDirectory" );
TCHAR ParamsXmlFileNameKey[]     = TEXT( "ParamsXmlFileName" );
TCHAR PollMsKey[]                = TEXT( "PollMs" );
TCHAR StatFileNameKey[]          = TEXT( "StatFileName" );
TCHAR DataFileNameKey[]          = TEXT( "DataFileName" );
TCHAR ShotDataFileNameKey[]      = TEXT( "ShotDataFileName" );
TCHAR WindowRectangleKey[]       = TEXT( "WindowRectangle" );
TCHAR MonallIniFile[]            = TEXT( "monall.ini" );  /* I need this for ftii_parameters.cpp */
UINT  PollMs;
STRING_CLASS RootSorcDirectory;
STRING_CLASS RootSymphonyDirectory;
STRING_CLASS ParamsXmlFileName;
STRING_CLASS StatFileName;
STRING_CLASS DataFileName;
STRING_CLASS ShotDataFileName;
MACHINE_CLASS   CurrentMachine;
PART_CLASS      CurrentPart;
FTANALOG_CLASS  CurrentFtAnalog;
PARAMETER_CLASS CurrentParam;

#else

extern TCHAR ConfigSection[];
extern TCHAR RootSymphonyDirectoryKey[];
extern TCHAR ParamsXmlFileNameKey[];
extern TCHAR PollMsKey[];
extern TCHAR StatFileNameKey[];
extern TCHAR DataFileNameKey[];
extern TCHAR ShotDataFileNameKey[];
extern TCHAR WindowRectangleKey[];
extern TCHAR PostDataIniFileName[];
extern UINT  PollMs;
extern STRING_CLASS RootSorcDirectory;
extern STRING_CLASS RootSymphonyDirectory;
extern STRING_CLASS ParamsXmlFileName;
extern STRING_CLASS StatFileName;
extern STRING_CLASS DataFileName;
extern STRING_CLASS ShotDataFileName;
extern MACHINE_CLASS CurrentMachine;
extern PART_CLASS    CurrentPart;
extern FTANALOG_CLASS CurrentFtAnalog;
extern PARAMETER_CLASS CurrentParam;
#endif