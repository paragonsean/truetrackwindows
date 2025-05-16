#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\genlist.h"
#include "..\include\fileclas.h"
#include "..\include\subs.h"
#include "..\include\msgclas.h"

static TCHAR EmptyString[] = TEXT("");
const static TCHAR CommaChar = TEXT( ',' );

/***********************************************************************
*                              CLEANUP                                 *
***********************************************************************/
void BLOCK_MESSAGE_CLASS::cleanup( void )
{
BLOCK_MESSAGE_ENTRY * bme;

msglist.rewind();
while ( TRUE )
    {
    bme = (BLOCK_MESSAGE_ENTRY *) msglist.next();
    if ( !bme )
        break;
    if ( bme->msg )
        delete[] bme->msg;
    delete bme;
    }

msglist.remove_all();
}


/***********************************************************************
*                          BLOCK_MESSAGE_CLASS                         *
***********************************************************************/
BLOCK_MESSAGE_CLASS::BLOCK_MESSAGE_CLASS( void )
{
}

/***********************************************************************
*                         ~BLOCK_MESSAGE_CLASS                         *
***********************************************************************/
BLOCK_MESSAGE_CLASS::~BLOCK_MESSAGE_CLASS( void )
{
cleanup();
}

/***********************************************************************
*                               LOAD                                   *
***********************************************************************/
BOOLEAN BLOCK_MESSAGE_CLASS::load( TCHAR * filename )
{
BOOLEAN status;
int32   slen;
int32   b1;
int32   b2;

TCHAR * cp;
FILE_CLASS   f;
BLOCK_MESSAGE_ENTRY * bme;

cleanup();

status = FALSE;
if ( f.open_for_read(filename)  )
    {
    while ( TRUE )
        {
        cp = f.readline();
        if ( !cp )
            {
            status = TRUE;
            break;
            }
        slen = lstrlen( cp );
        if ( replace_char_with_null(cp, CommaChar) )
            {
            b1 = asctoint32( cp );
            cp = nextstring( cp );
            if ( replace_char_with_null(cp, CommaChar) )
                {
                b2 = asctoint32( cp );
                cp = nextstring( cp );
                slen = lstrlen( cp );
                if ( slen > 0 )
                    {
                    bme = new BLOCK_MESSAGE_ENTRY;
                    if ( !bme )
                        break;
                    bme->low_block_number  = b1;
                    bme->high_block_number = b2;
                    bme->msg = maketext( slen );
                    if ( !bme->msg )
                        {
                        delete bme;
                        break;
                        }
                    lstrcpy( bme->msg, cp );
                    msglist.append( bme );
                    }
                }
            }
        }
    f.close();
    }

return status;
}

/***********************************************************************
*                              MESSAGE                                 *
***********************************************************************/
TCHAR * BLOCK_MESSAGE_CLASS::message( int32 block_number )
{
BLOCK_MESSAGE_ENTRY * bme;
msglist.rewind();
while ( TRUE )
    {
    bme = (BLOCK_MESSAGE_ENTRY *) msglist.next();
    if ( !bme )
        break;

    if ( block_number >= bme->low_block_number && block_number <= bme->high_block_number )
        return bme->msg;
    }

return EmptyString;
}