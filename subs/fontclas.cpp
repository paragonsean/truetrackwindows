#include <windows.h>

#include "..\include\fontclas.h"

const TCHAR CommaChar = TEXT( ',' );
const TCHAR MinusChar = TEXT( '-' );
const TCHAR NullChar  = TEXT( '\0' );

/***********************************************************************
*                               REVERSE                                *
***********************************************************************/
static void reverse( TCHAR * s )
{

TCHAR  c;
TCHAR * cp1;
TCHAR * cp2;

cp1 = s;
cp2 = s + lstrlen(s) - 1;
while ( cp1 < cp2 )
    {
    c = *cp2;
    *cp2-- = *cp1;
    *cp1++ = c;
    }
}

/***********************************************************************
*                            LONG_FROM_TEXT                            *
***********************************************************************/
static long long_from_text( const TCHAR * sorc )
{
const MAX_INTEGER_LEN = 18;
TCHAR sc[MAX_INTEGER_LEN+1];
char  save[MAX_INTEGER_LEN+1];
int   n;
long  x;

x = 0;
n = lstrlen( sorc );
if ( n > MAX_INTEGER_LEN )
    n = MAX_INTEGER_LEN;

if ( n > 0 )
    {
    lstrcpyn( sc, sorc, n+1 );
    CharToOem( sc, save );
    x = strtol( save, NULL, 10 );
    }

return x;
}

/***********************************************************************
*                               ascii_long                             *
***********************************************************************/
static TCHAR * ascii_long( long n, long radix )
{
const int MAXLEN = 18;
static TCHAR buf[MAXLEN+1];

TCHAR * cp;
long    sign;
long    i;

sign = n;
if ( n < 0 )
    n = -n;

cp = buf;
do  {
    i = n % radix;
    if ( i <= 9 )
        *cp = TCHAR(i+'0');
    else
        *cp = TCHAR(i+'7');
    cp++;
    n /= radix;
    } while ( n > 0 );

if ( sign < 0 )
    *cp++ = MinusChar;

*cp = NullChar;

reverse(buf);
return buf;
}

/***********************************************************************
*                              ASCII_LONG                              *
***********************************************************************/
static TCHAR * ascii_long( long n )
{
return ascii_long( n, 10 );
}

/***********************************************************************
*                           GOTO_NEXT_STRING                           *
***********************************************************************/
static TCHAR * goto_next_string( TCHAR * cp )
{
while ( *cp != NullChar )
    cp++;
cp++;
return cp;
}

/***********************************************************************
*                             COPY_W_COMMA                             *
***********************************************************************/
static TCHAR * copy_w_comma( TCHAR * dest, TCHAR * sorc )
{

while ( *sorc != NullChar )
    {
    *dest = *sorc;
    dest++;
    sorc++;
    }
*dest = CommaChar;
dest++;

return dest;
}

/***********************************************************************
*                            FONT_CLASS                                *
***********************************************************************/
FONT_CLASS::FONT_CLASS()
{

lfHeight         = 0;
lfWidth          = 0;
lfEscapement     = 0;
lfOrientation    = 0;
lfWeight         = 0;
lfItalic         = 0;
lfUnderline      = 0;
lfStrikeOut      = 0;
lfCharSet        = 0;
lfOutPrecision   = 0;
lfClipPrecision  = 0;
lfQuality        = 0;
lfPitchAndFamily = 0;
*lfFaceName = TEXT( '\0' );
}

/***********************************************************************
*                             FONT_CLASS                               *
*                                 =                                    *
***********************************************************************/
void FONT_CLASS::operator=(const LOGFONT & sorc )
{
lfHeight         = sorc.lfHeight;
lfWidth          = sorc.lfWidth;
lfEscapement     = sorc.lfEscapement;
lfOrientation    = sorc.lfOrientation;
lfWeight         = sorc.lfWeight;
lfItalic         = sorc.lfItalic;
lfUnderline      = sorc.lfUnderline;
lfStrikeOut      = sorc.lfStrikeOut;
lfCharSet        = sorc.lfCharSet;
lfOutPrecision   = sorc.lfOutPrecision;
lfClipPrecision  = sorc.lfClipPrecision;
lfQuality        = sorc.lfQuality;
lfPitchAndFamily = sorc.lfPitchAndFamily;

lstrcpyn( lfFaceName, sorc.lfFaceName, LF_FACESIZE );
}

/***********************************************************************
*                            FONT_CLASS                                *
***********************************************************************/
FONT_CLASS::FONT_CLASS( const LOGFONT & sorc )
{
*this = sorc;
}

/***********************************************************************
*                            FONT_CLASS                                *
***********************************************************************/
FONT_CLASS::FONT_CLASS( const FONT_CLASS & sorc )
{
*this = (LOGFONT) sorc;
}

/***********************************************************************
*                            ~FONT_CLASS                               *
***********************************************************************/
FONT_CLASS::~FONT_CLASS()
{
}

/***********************************************************************
*                             FONT_CLASS                               *
*                                 =                                    *
***********************************************************************/
void FONT_CLASS::operator=(const FONT_CLASS & sorc )
{
*this = (LOGFONT) sorc;
}

/***********************************************************************
*                             FONT_CLASS                               *
*                                 ==                                   *
***********************************************************************/
bool FONT_CLASS::operator==(const LOGFONT & sorc )
{
if ( lfHeight != sorc.lfHeight )
    return FALSE;

if ( lfWidth != sorc.lfWidth )
    return FALSE;

if ( lfEscapement != sorc.lfEscapement )
    return FALSE;

if ( lfOrientation != sorc.lfOrientation )
    return FALSE;

if ( lfWeight != sorc.lfWeight )
    return FALSE;

if ( lfItalic != sorc.lfItalic )
    return FALSE;

if ( lfUnderline != sorc.lfUnderline )
    return FALSE;

if ( lfStrikeOut != sorc.lfStrikeOut )
    return FALSE;

if ( lfCharSet != sorc.lfCharSet )
    return FALSE;

if ( lfOutPrecision != sorc.lfOutPrecision )
    return FALSE;

if ( lfClipPrecision != sorc.lfClipPrecision )
    return FALSE;

if ( lfQuality != sorc.lfQuality )
    return FALSE;

if ( lfPitchAndFamily!= sorc.lfPitchAndFamily )
    return FALSE;

if ( lstrcmp(lfFaceName, sorc.lfFaceName) != 0 )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                             FONT_CLASS                               *
*                                 ==                                   *
***********************************************************************/
bool FONT_CLASS::operator==(const FONT_CLASS & sorc )
{
return (*this == (LOGFONT) sorc);
}

/***********************************************************************
*                                  GET                                 *
***********************************************************************/
bool FONT_CLASS::get( TCHAR *sorc )
{
const int MAXLEN = 130 + LF_FACESIZE;

TCHAR * s;
TCHAR * cp;
int     nof_commas;
int     n;
bool    status;

if ( !sorc )
    return false;

n = lstrlen( sorc );

/*
-----------------------------------------------------------
If the string is longer than MAXLEN then something is wrong
----------------------------------------------------------- */
if ( n > MAXLEN )
    return false;

s = new TCHAR[n+1];
if ( !s )
    return false;

status = true;
lstrcpy( s, sorc );

/*
---------------------------------
Replace all the commas with nulls
--------------------------------- */
nof_commas = 0;
n          = MAXLEN;
cp         = s;
while ( *cp != NullChar && n > 0 )
    {
    if ( *cp == CommaChar )
        {
        *cp = NullChar;
        nof_commas++;
        }
    cp++;
    n--;
    }

/*
-----------------------------------------------------------------
If the number of commas is not correct then I would do bad things
----------------------------------------------------------------- */
if ( nof_commas != 13 )
    status = false;

if ( status )
    {
    cp = s;

    lfHeight = (LONG) long_from_text( cp );
    cp = goto_next_string( cp );

    lfWidth = (LONG) long_from_text( cp );
    cp = goto_next_string( cp );

    lfEscapement = (LONG) long_from_text( cp );
    cp = goto_next_string( cp );

    lfOrientation = (LONG) long_from_text( cp );
    cp = goto_next_string( cp );

    lfWeight = (LONG) long_from_text( cp );
    cp = goto_next_string( cp );

    lfItalic = (BYTE) long_from_text( cp );
    cp = goto_next_string( cp );

    lfUnderline = (BYTE) long_from_text( cp );
    cp = goto_next_string( cp );

    lfStrikeOut = (BYTE) long_from_text( cp );
    cp = goto_next_string( cp );

    lfCharSet = (BYTE) long_from_text( cp );
    cp = goto_next_string( cp );

    lfOutPrecision = (BYTE) long_from_text( cp );
    cp = goto_next_string( cp );

    lfClipPrecision  = (BYTE) long_from_text( cp );
    cp = goto_next_string( cp );

    lfQuality = (BYTE) long_from_text( cp );
    cp = goto_next_string( cp );

    lfPitchAndFamily = (BYTE) long_from_text( cp );
    cp = goto_next_string( cp );
    lstrcpy( lfFaceName, cp );
    }

delete[] s;
return status;
}

/***********************************************************************
*                                PUT                                   *
***********************************************************************/
TCHAR * FONT_CLASS::put()
{
const int BUFLEN = 130 + LF_FACESIZE;
static TCHAR buf[BUFLEN+1];
TCHAR * cp;

cp = copy_w_comma( buf, ascii_long((long) lfHeight)         );
cp = copy_w_comma( cp,  ascii_long((long) lfWidth)          );
cp = copy_w_comma( cp,  ascii_long((long) lfEscapement)     );
cp = copy_w_comma( cp,  ascii_long((long) lfOrientation)    );
cp = copy_w_comma( cp,  ascii_long((long) lfWeight)         );
cp = copy_w_comma( cp,  ascii_long((long) lfItalic)         );
cp = copy_w_comma( cp,  ascii_long((long) lfUnderline)      );
cp = copy_w_comma( cp,  ascii_long((long) lfStrikeOut)      );
cp = copy_w_comma( cp,  ascii_long((long) lfCharSet)        );
cp = copy_w_comma( cp,  ascii_long((long) lfOutPrecision)   );
cp = copy_w_comma( cp,  ascii_long((long) lfClipPrecision)  );
cp = copy_w_comma( cp,  ascii_long((long) lfQuality)        );
cp = copy_w_comma( cp,  ascii_long((long) lfPitchAndFamily) );
lstrcpy( cp, lfFaceName );

return buf;
}

/***********************************************************************
*                                CHOOSE                                *
***********************************************************************/
bool FONT_CLASS::choose( HWND w, COLORREF & color )
{
CHOOSEFONT cf;

cf.lStructSize    = sizeof( CHOOSEFONT );
cf.hwndOwner      = w;
cf.hDC            = 0;
cf.lpLogFont      = this;
cf.iPointSize     = 0;
cf.rgbColors      = (DWORD) color;
cf.Flags          = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_EFFECTS;
cf.lCustData      = 0;
cf.lpfnHook       = 0;
cf.lpTemplateName = 0;
cf.hInstance      = 0;
cf.lpszStyle      = 0;
cf.nFontType      = 0;
cf.nSizeMin       = 0;
cf.nSizeMax       = 0;

if ( ChooseFont(&cf) )
    {
    color = (COLORREF) cf.rgbColors;
    return true;
    }

return false;
}

/***********************************************************************
*                                CHOOSE                                *
***********************************************************************/
bool FONT_CLASS::choose( HWND w )
{
CHOOSEFONT cf;

cf.lStructSize    = sizeof( CHOOSEFONT );
cf.hwndOwner      = w;
cf.hDC            = 0;
cf.lpLogFont      = this;
cf.iPointSize     = 0;
cf.rgbColors      = 0;
cf.Flags          = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
cf.lCustData      = 0;
cf.lpfnHook       = 0;
cf.lpTemplateName = 0;
cf.hInstance      = 0;
cf.lpszStyle      = 0;
cf.nFontType      = 0;
cf.nSizeMin       = 0;
cf.nSizeMax       = 0;

if ( ChooseFont(&cf) )
    {
    return true;
    }

return false;
}


/***********************************************************************
*                           FONT_LIST_ENTRY                            *
***********************************************************************/
FONT_LIST_ENTRY::FONT_LIST_ENTRY()
{
next            = 0;
reference_count = 0;
handle          = INVALID_FONT_HANDLE;
}

/***********************************************************************
*                          ~FONT_LIST_ENTRY                            *
***********************************************************************/
FONT_LIST_ENTRY::~FONT_LIST_ENTRY()
{
if ( handle != INVALID_FONT_HANDLE )
    {
    DeleteObject( handle );
    handle = INVALID_FONT_HANDLE;
    }
}

/***********************************************************************
*                           FONT_LIST_CLASS                            *
***********************************************************************/
FONT_LIST_CLASS::FONT_LIST_CLASS()
{
first = 0;
}

/***********************************************************************
*                          ~FONT_LIST_CLASS                            *
***********************************************************************/
FONT_LIST_CLASS::~FONT_LIST_CLASS()
{
FONT_LIST_ENTRY * dead_entry;

while ( first )
    {
    dead_entry = first;
    first = first->next;
    delete dead_entry;
    }
}

/***********************************************************************
*                           FONT_LIST_CLASS                            *
*                               GET_FONT                               *
***********************************************************************/
bool FONT_LIST_CLASS::get_font( FONT_CLASS & dest, HFONT handle_to_get )
{
FONT_LIST_ENTRY * f;

f = first;
while ( f )
    {
    if ( f->handle == handle_to_get )
        {
        dest = f->font;
        return true;
        }
    f = f->next;
    }

return false;
}

/***********************************************************************
*                           FONT_LIST_CLASS                            *
*                              GET_HANDLE                              *
* Each call to this function should be balanced with a call to         *
* free_handle.                                                         *
***********************************************************************/
HFONT FONT_LIST_CLASS::get_handle( const FONT_CLASS & font_desired )
{
FONT_LIST_ENTRY * f;
FONT_LIST_ENTRY * newfont;

f = first;
while ( f )
    {
    if ( f->font == font_desired )
        {
        f->reference_count++;
        return f->handle;
        }
    f = f->next;
    }

/*
---------------------------------------------
If I get here there is no entry for this font
--------------------------------------------- */
newfont = new FONT_LIST_ENTRY;
if ( newfont )
    {
    newfont->font   = font_desired;
    newfont->handle = CreateFontIndirect( &newfont->font );
    if ( newfont->handle )
        {
        newfont->reference_count = 1;
        if ( first )
            {
            f = first;
            while ( f->next )
                f = f->next;
            f->next = newfont;
            }
        else
            {
            first = newfont;
            }
        return newfont->handle;
        }
    delete newfont;
    newfont = 0;
    }

/*
------------------------------------------------------------------
I shouldn't ever get here but the following is harmless as it will
never be in my list.
------------------------------------------------------------------ */
return (HFONT) GetStockObject( DEFAULT_GUI_FONT );
}

/***********************************************************************
*                           FONT_LIST_CLASS                            *
*                             FREE_HANDLE                              *
***********************************************************************/
void FONT_LIST_CLASS::free_handle( HFONT handle_to_free )
{
FONT_LIST_ENTRY * prev;
FONT_LIST_ENTRY * f;

f    = first;
prev = 0;
while ( f )
    {
    if ( f->handle == handle_to_free )
        {
        f->reference_count--;
        if ( f->reference_count <= 0 )
            {
            if ( prev )
                prev->next = f->next;
            else
                first = f->next;
            delete f;
            }
        break;
        }

    prev = f;
    f    = f->next;
    }
}

