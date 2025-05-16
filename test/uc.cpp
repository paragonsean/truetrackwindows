#include <windows.h>
#include <conio.h>

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
WritePrivateProfileString( TEXT("Config"),TEXT("UnicodeSet"), TEXT("0Cvda"), TEXT(".\\display.ini") );
return 0;
}
