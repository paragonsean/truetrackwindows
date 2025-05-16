#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"

#define MAX_MENU_NAME_LEN 80
#define MAX_LINE_LEN      80

#define TOP_TYPE       0
#define POPUP_TYPE     1
#define SEPARATOR_TYPE 2

static TCHAR  CommaChar = TEXT( ',' );
static TCHAR  NullChar  = TEXT( '\0' );
static HANDLE Fh        = INVALID_HANDLE_VALUE;
static HMENU  MainMenu  = 0;
static HMENU  PopupMenu = 0;
static HWND   ParentWindow = 0;

class MENU_ENTRY
    {
    public:

    short type;
    TCHAR name[MAX_MENU_NAME_LEN];
    UINT  id;
    short password_level;

    MENU_ENTRY( void );
    void operator=(MENU_ENTRY &);
    ~MENU_ENTRY( void );
    };

MENU_ENTRY Me;

/***********************************************************************
*                              MENU_ENTRY                              *
***********************************************************************/
MENU_ENTRY::MENU_ENTRY( void )
{
type           = TOP_TYPE;
*name          = NullChar;
id             = 0;
password_level = 0;
}

/***********************************************************************
*                             ~MENU_ENTRY                              *
***********************************************************************/
MENU_ENTRY::~MENU_ENTRY( void )
{
}

/***********************************************************************
*                             OPERATOR =                               *
***********************************************************************/
void MENU_ENTRY::operator=(MENU_ENTRY & sorc )
{
type = sorc.type;
lstrcpy( name, sorc.name );
id   = sorc.id;
password_level = sorc.password_level;
}

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
void cleanup( void )
{
if ( Fh != INVALID_HANDLE_VALUE )
    {
    CloseHandle( Fh );
    Fh = INVALID_HANDLE_VALUE;
    }

if ( MainMenu )
    {
    DestroyMenu( MainMenu );
    MainMenu = 0;
    }

if ( PopupMenu )
    {
    DestroyMenu( PopupMenu );
    PopupMenu = 0;
    }

}

/***********************************************************************
*                            OPEN_MENU_FILE                            *
***********************************************************************/
BOOL open_menu_file( TCHAR * menu_name )
{
TCHAR path[MAX_PATH+1];
DWORD access_mode;
DWORD share_mode;

access_mode = GENERIC_READ;
share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;

get_exe_directory( path );
lstrcat( path, menu_name );
lstrcat( path, TEXT(".mnu") );

Fh = CreateFile( path, access_mode, share_mode, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( Fh == INVALID_HANDLE_VALUE )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            REMOVE_COMMAS                             *
***********************************************************************/
BOOL remove_commas( TCHAR * buf )
{
TCHAR * cp;
int     count;

cp = buf;
count = 0;

while ( *cp )
    {
    if ( *cp == CommaChar )
        {
        count++;
        *cp = NullChar;
        }
    cp++;
    }

if ( count == 3 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            READ_NEXT_LINE                            *
***********************************************************************/
static BOOL read_next_line( void )
{
short   slen;
TCHAR * cp;
TCHAR   buf[MAX_LINE_LEN+1];

slen = readline( buf, Fh, MAX_LINE_LEN );
if ( slen < 2 )
    slen = readline( buf, Fh, MAX_LINE_LEN );

if ( slen < 2 )
    return FALSE;

if ( !remove_commas(buf) )
    return FALSE;

cp = buf;
Me.type = asctoint32( cp );
cp += lstrlen(cp) + 1;

lstrcpy( Me.name, cp );
cp += lstrlen(cp) + 1;

Me.id = asctoint32( cp );
cp += lstrlen(cp) + 1;

Me.password_level = asctoint32( cp );

return TRUE;
}

/***********************************************************************
*                              LOAD_MENU                               *
***********************************************************************/
BOOL load_menu( HWND w, TCHAR * menuname, short current_password_level, const UINT * bad_ids, int nof_bad_ids )
{

int   i;
bool  is_bad;
BOOL  have_top_menu;
int32 top_count;
int32 popup_count;
MENU_ENTRY topmenu;
HMENU m;

/*
-----------------------------------
Get the handle of the existing menu
----------------------------------- */
m = GetMenu( w );
if ( m )
    {
    /*
    ----------------------------
    Set the current menu to NULL
    ---------------------------- */
    SetMenu( w, NULL );

    /*
    -------------------------
    Destroy the existing menu
    ------------------------- */
    DestroyMenu( m );
    }

have_top_menu = FALSE;
top_count     = 0;
MainMenu      = CreateMenu();

if ( MainMenu == NULL )
    {
    return FALSE;
    }

/*
------------------
Open the menu file
------------------ */
if ( !open_menu_file(menuname) )
    {
    cleanup();
    return FALSE;
    }

popup_count = 0;

while ( read_next_line() )
    {
    if ( Me.type == TOP_TYPE )
        {
        if ( have_top_menu && popup_count > 0 )
            {
            AppendMenu( MainMenu, MF_POPUP, (UINT) PopupMenu, topmenu.name );
            top_count++;
            PopupMenu = 0;
            have_top_menu = FALSE;
            }

        if ( Me.password_level <= current_password_level )
            {
            have_top_menu = TRUE;
            topmenu = Me;
            popup_count = 0;
            }
        }
    else if ( have_top_menu )
        {
        if ( Me.type == POPUP_TYPE )
            {
            is_bad = false;
            if ( nof_bad_ids > 0 )
                {
                for ( i=0; i<nof_bad_ids; i++ )
                    {
                    if ( Me.id == bad_ids[i] )
                        {
                        is_bad = true;
                        break;
                        }
                    }
                }

            if ( Me.password_level <= current_password_level && !is_bad )
                {
                if ( popup_count == 0 )
                    PopupMenu = CreateMenu();
                popup_count++;
                AppendMenu( PopupMenu, MF_STRING, Me.id, Me.name );
                }
            }
        else if ( Me.type == SEPARATOR_TYPE )
            {
            if ( PopupMenu && popup_count > 0 &&  Me.password_level <= current_password_level )
                AppendMenu( PopupMenu, MF_SEPARATOR, 0, NULL );
            }
        }
    }

CloseHandle( Fh );
Fh = INVALID_HANDLE_VALUE;

if ( top_count > 0 )
    {
    SetMenu( w, MainMenu );
    ParentWindow = w;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                              LOAD_MENU                               *
***********************************************************************/
BOOL load_menu( HWND w, TCHAR * menuname, short current_password_level )
{
return load_menu( w, menuname, current_password_level, 0, 0 );
}

/***********************************************************************
*                            ADD_TO_MENU                               *
***********************************************************************/
void add_to_menu( UINT id, TCHAR * name )
{
if ( MainMenu && ParentWindow )
    {
    AppendMenu( MainMenu, MF_STRING, id, name );
    DrawMenuBar( ParentWindow );
    }
}


/***********************************************************************
*                          REMOVE_FROM_MENU                            *
***********************************************************************/
void remove_from_menu( UINT id )
{
if ( MainMenu && ParentWindow )
    {
    RemoveMenu( MainMenu, id, MF_BYCOMMAND );
    DrawMenuBar( ParentWindow );
    }
}

/***********************************************************************
*                            MODIFY_MENU_STRING                        *
***********************************************************************/
void modify_menu_string( UINT id, TCHAR * newname )
{
if ( MainMenu && ParentWindow )
    {
    ModifyMenu( MainMenu, id, MF_STRING, id, newname );
    DrawMenuBar( ParentWindow );
    }
}

