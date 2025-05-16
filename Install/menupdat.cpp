#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\computer.h"
#include "..\include\fileclas.h"
#include "..\include\names.h"
#include "..\include\subs.h"

static TCHAR TEMP_FILE_NAME[] = TEXT( "visimenu.tmp" );
static TCHAR CommaChar        = TEXT( ',' );
static TCHAR BackSlashChar    = TEXT( '\\' );

const int32 NO_MENU_ID = 0xffff;

class NEW_MENU_ENTRY
    {
    public:

    int32 id_to_follow;
    int32 id;
    TCHAR * s;

    void clear( void );
    NEW_MENU_ENTRY() { id_to_follow = NO_MENU_ID; id = NO_MENU_ID; s = 0; }
    ~NEW_MENU_ENTRY() { clear(); }
    BOOLEAN get( TCHAR * sorc );
    };

/***********************************************************************
*                               MENU_ID                                *
***********************************************************************/
int32 menu_id( TCHAR * sorc )
{
TCHAR * cp;

cp = findchar( CommaChar, sorc );
if ( cp )
    {
    cp++;
    cp = findchar( CommaChar, cp );
    }

if ( cp )
    {
    cp++;
    return asctoint32( cp );
    }

return NO_MENU_ID;
}

/***********************************************************************
*                            NEW_MENU_ENTRY                            *
*                                CLEAR                                 *
***********************************************************************/
void NEW_MENU_ENTRY::clear( void )
{
id_to_follow = NO_MENU_ID;
id = NO_MENU_ID;
if ( s )
    delete[] s;
s = 0;
}

/***********************************************************************
*                            NEW_MENU_ENTRY                            *
*                                 GET                                  *
***********************************************************************/
BOOLEAN NEW_MENU_ENTRY::get( TCHAR * sorc )
{
TCHAR * cp;

clear();

cp = findchar( CommaChar, sorc );
if ( cp )
    {
    cp++;
    if ( countchars(CommaChar, cp) == 3 )
        {
        id_to_follow = asctoint32( sorc );
        s = maketext( cp );
        id = menu_id( s );
        }

    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                             MENU_UPDATE                              *
*                                                                      *
* Each entry has to have a menu id followed by the new line to         *
* be put into the file after the id in question.                       *
*                                                                      *
* e.g. Say you want to add a new multiple print item as follows        *
*                                                                      *
*                  1,&Print Multiple Shots,119,0                       *
*                                                                      *
*      and say you want it to follow the existing entry                *
*                                                                      *
*                        1,&Print,117,0                                *
*                                                                      *
*      in the newmenu.mnu file put                                     *
*                                                                      *
*                 117,1,&Print Multiple Shots,119,0                    *
*                                                                      *
***********************************************************************/
BOOLEAN menu_update( TCHAR * sorc_file_name, TCHAR * dest_file_name )
{
FILE_CLASS sf;
FILE_CLASS df;
int32      this_id;
int32      i;
int32      n;
TCHAR    * cp;
BOOLEAN    have_new_file;
TCHAR    * tempfile;

NEW_MENU_ENTRY * newlist;

/*
--------------------------------------------------
Read the new file to see how many lines there are.
-------------------------------------------------- */
n = 0;
sf.open_for_read( sorc_file_name );
while ( TRUE )
    {
    cp = sf.readline();
    if ( !cp )
        break;
    if ( countchars(CommaChar, cp) != 4 )
        break;

    n++;
    }

sf.close();

/*
-------------
Nothing to do
------------- */
if ( n == 0 )
    return TRUE;

/*
------------------------------
Create an array of new entries
------------------------------ */
newlist = new NEW_MENU_ENTRY[n];
if ( !newlist )
    return FALSE;


/*
---------------------------------
Make a list of the new menu items
--------------------------------- */
sf.open_for_read( sorc_file_name );
for ( i=0; i<n; i++ )
    {
    cp = sf.readline();
    if ( !cp )
        break;

    if ( countchars(CommaChar, cp) != 4 )
        break;

    if ( !newlist[i].get(cp) )
        {
        n = i;
        break;
        }
    }

sf.close();

/*
-----------------------------------------
Ignore any new entries that already exist
----------------------------------------- */
sf.open_for_read( dest_file_name );
while ( TRUE )
    {
    cp = sf.readline();
    if ( !cp )
        break;
    if ( countchars(CommaChar, cp) != 3 )
        break;
    this_id = menu_id( cp );
    for ( i=0; i<n; i++ )
        {
        if ( this_id == newlist[i].id )
            {
            newlist[i].id_to_follow = NO_MENU_ID;
            }
        }

    }

/*
--------------------------------------------------------------
Make a name for the temporary file in the same dir as the dest
-------------------------------------------------------------- */
tempfile = new TCHAR[MAX_PATH+1];
lstrcpy( tempfile, dest_file_name );
cp = tempfile + lstrlen(tempfile);
cp--;

while ( cp > tempfile )
    {
    if ( *cp == BackSlashChar )
        {
        cp++;
        lstrcpy( cp, TEMP_FILE_NAME );
        break;
        }
    cp--;
    }

have_new_file = FALSE;
sf.rewind();
if ( df.open_for_write(tempfile) )
    {
    while ( TRUE )
        {
        cp = sf.readline();
        if ( !cp )
            break;
        if ( countchars(CommaChar, cp) == 3 )
            {
            df.writeline( cp );
            this_id = menu_id( cp );
            for ( i=0; i<n; i++ )
                {
                if ( this_id == newlist[i].id_to_follow )
                    {
                    df.writeline( newlist[i].s );
                    /*
                    ----------------------------------------------
                    The next new entry may want to follow this one
                    so update this_id with the new one.
                    ----------------------------------------------  */
                    this_id = newlist[i].id;
                    have_new_file = TRUE;
                    }
                }
            }
        }

    df.close();
    }
sf.close();

if ( have_new_file )
    {
    DeleteFile( dest_file_name );
    MoveFile( tempfile, dest_file_name );
    }

for ( i=0; i<n; i++ )
    newlist[i].clear();

delete[] newlist;

delete[] tempfile;

return TRUE;
}
