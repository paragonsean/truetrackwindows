#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\verrors.h"
#include "..\include\subs.h"

#define MAX_ERROR_MSG_LEN 60

static TCHAR Msg[MAX_ERROR_MSG_LEN+1];
static TCHAR RoutinesFile[] = TEXT("routines.ini");
static TCHAR ErrMsgFile[]   = TEXT("errormsg.ini");
static TCHAR FtErrMsgFile[] = TEXT("fterrors.ini");

/***********************************************************************
*                             WAIT_FOR_KEY                             *
***********************************************************************/
static void wait_for_key( void )
{
MessageBox( NULL, Msg, TEXT("Error"), MB_OK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION );
}

/***********************************************************************
*                              ADD_STRING                              *
***********************************************************************/
static void add_string( TCHAR * sorc )
{

TCHAR * dest;
short  slen;

slen = lstrlen(Msg);
dest = Msg + slen;
slen = MAX_ERROR_MSG_LEN - slen;

if ( slen <= 1 )
    return;

/*
----------------------------------------------------
If this is not the start of the message, add a space
---------------------------------------------------- */
if ( *Msg )
    {
    *dest = ' ';
    dest++;
    slen--;
    }

lstrcpyn( dest, sorc, slen );
*(dest+slen) = '\0';

}

/***********************************************************************
*                            ADD_ERR_NUMBER                            *
***********************************************************************/
static void add_err_number( TCHAR * filename, short error_number )
{

TCHAR * cp;
TCHAR   ascii_error_number[MAX_INTEGER_LEN+1];

int32toasc( ascii_error_number, error_number, DECIMAL_RADIX );
cp = get_ini_string( filename, TEXT("VisiErrors"), ascii_error_number );
if ( cp && lstrcmp(cp, UNKNOWN) != 0 )
    {
    add_string( cp );
    }
else
    {
    add_string( filename );
    add_string( TEXT(" ") );
    add_string( ascii_error_number );
    }

}

/***********************************************************************
*                            ERROR_MESSAGE                             *
***********************************************************************/
void error_message( TCHAR * s )
{

*Msg = '\0';
add_string( s );
wait_for_key();
}

/***********************************************************************
*                            ERROR_MESSAGE                             *
***********************************************************************/
void error_message( short routine_number, short msg_number, TCHAR * s )
{

*Msg = '\0';

add_err_number( RoutinesFile, routine_number );
add_err_number( ErrMsgFile,   msg_number );
add_string( s );
wait_for_key();

}

/***********************************************************************
*                            ERROR_MESSAGE                             *
***********************************************************************/
void error_message( short routine_number, short msg_number, short n )
{

TCHAR ascii_number[12];

*Msg = TEXT('\0');

add_err_number( RoutinesFile, routine_number );
add_err_number( ErrMsgFile,   msg_number );
add_string( TEXT("=") );

if ( msg_number == FT_BOARD_ERROR )
    add_err_number( FtErrMsgFile, n );
else
    {
    *ascii_number = '[';
    int32toasc( ascii_number+1, n, DECIMAL_RADIX );
    lstrcat( ascii_number, TEXT("]") );
    add_string( ascii_number );
    }

wait_for_key();
}

/***********************************************************************
*                            ERROR_MESSAGE                             *
***********************************************************************/
void error_message( short routine_number, short msg_number )
{

*Msg = '\0';

add_err_number( RoutinesFile, routine_number );
add_err_number( ErrMsgFile,   msg_number );

wait_for_key();
}

/***********************************************************************
*                            ERROR_MESSAGE                             *
***********************************************************************/
void error_message( TCHAR * s, short msg_number )
{

*Msg = '\0';
add_string( s );
add_err_number( ErrMsgFile,   msg_number );

wait_for_key();
}

