#include <windows.h>
#include <conio.h>

#include "..\include\strarray.h"

const TCHAR SpaceString[] = TEXT( " ");
static TCHAR NullChar = TEXT( '\0' );

/***********************************************************************
*                             REMOVE_CHAR                              *
***********************************************************************/
void remove_char( TCHAR * sorc )
{
TCHAR * dest;

dest = sorc;
while ( TRUE )
    {
    sorc++;
    *dest = *sorc;
    if ( *sorc == NullChar )
        break;
    dest++;
    }
}

/***********************************************************************
*                               FINDSTRING                             *
*          Return pointer to first occurance of s_to_find in s.        *
***********************************************************************/
TCHAR * findstring( const TCHAR * s_to_find, TCHAR * s )
{
const TCHAR * fs;
TCHAR * cp;
TCHAR sorc_char;
TCHAR dest_char;

if ( !s )
    return 0;

if ( !s_to_find )
    return 0;

while ( (*s) != NullChar )
    {
    fs = s_to_find;
    cp = s;
    while ( TRUE )
        {
        if ( *fs == NullChar )
            return s;
        sorc_char = (TCHAR) CharUpper( (TCHAR *) *cp );
        dest_char = (TCHAR) CharUpper( (TCHAR *) *fs );
        if ( sorc_char != dest_char )
            break;

        cp++;
        fs++;
        }
    s++;
    }

return 0;
}

/***********************************************************************
*                               GETLINE                                *
***********************************************************************/
TCHAR * getline( void )
{
static TCHAR buf[103];
buf[0] = 100;

cgets( buf );

return buf+2;
}

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
// TCHAR   buf[103];

STRING_ARRAY_CLASS s;

s.add( "Visi-TraK" );
s.add( "FasTrak"   );
s.add( "Sure-Trak" );
s.add( "Hi you guys" );
 
s.rewind();
while ( s.next() )
   cprintf( "%s\n\r", s.text() );

if ( s.find( "Sure-Trak" ) )
    cprintf( "Found %s\n\r", s.text() );

cprintf( "n = %d\n\r", s.count() );

int i;
for ( i=0; i<s.count(); i++ )
    if ( s.find(i) )
        cprintf( "Found %d = %s\n\r", i, s.text() );

cprintf( "\n\r" );

for ( i=0; i<s.count(); i++ )
    cprintf( "Found [%d] = %s\n\r", i, s[i].text() );

cprintf( "\n\r" );

i = 1;
s[i] = "What's up";
cprintf( "Found [%d] = %s\n\r", i, s.text() );

cprintf( "\n\r" );

for ( i=0; i<s.count(); i++ )
    cprintf( "Found [%d] = %s\n\r", i, s[i].text() );

cprintf( "\n\r" );

while ( s.prev() )
    cprintf( "Previous = %s\n\r", s.text() );

s.rewind();
while ( s.remove() )
    ;

for ( i=0; i<s.count(); i++ )
    {
    if ( s.find(i) )
        cprintf( "Found %d = %s\n\r", i, s.text() );
    else
        cprintf( "There is NO number %d.\n\r", i );
    }

cprintf( "Input a line of tokens separated by comma+space: " );
s = getline();
if ( s.len() > 0 )
    s.split( TEXT( ", ") );

s.rewind();
while ( s.next() )
   cprintf( "[%s]\n\r", s.text() );

s.join( TEXT(" ") );
cprintf( "[%s]\n\r", s.text() );

s.split( TEXT( " ") );
s.rewind();
while ( s.next() )
   cprintf( "[%s]\n\r", s.text() );

s.join( SpaceString );
if ( s.contains("think") )
    cprintf( "\r\nThought is good" );
else
    cprintf( "\r\nNo Thought is good" );
//buf[0] = 10;
//cgets( buf );

return 0;
}

