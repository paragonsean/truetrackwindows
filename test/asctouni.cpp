#include <windows.h>
#include <conio.h>

#include "..\include\visiparm.h"
#include "..\include\structs.h"
#include "..\include\subs.h"

static const TCHAR NullChar        = TEXT( '\0' );
static const TCHAR PeriodChar      = TEXT( '.' );
static const TCHAR BackSlashChar   = TEXT( '\\' );
static const TCHAR ColonChar       = TEXT( ':' );
static TCHAR DefaultDirectory[]    = TEXT( "C:\\" );
static TCHAR StarDotStar[]         = TEXT( "*.*" );

struct WIDE_PROFILE_HEADER
    {
    TCHAR      machine_name[MACHINE_NAME_LEN+1+MACHINE_NAME_LEN+1];
    TCHAR      part_name[PART_NAME_LEN+1+PART_NAME_LEN+1];
    int32      shot_number;
    FILETIME   time_of_shot;      /* 8 bytes */
    short      n;
    short      last_pos_based_point; /* NofPosBasedPoints - 1 */
    CURVE_DATA curves[MAX_CURVES];
    };

WIDE_PROFILE_HEADER   Wph;

PROFILE_HEADER        Ph;
short                 NofParms;
float                 Parms[MAX_PARMS];

int                   NofPoints;
unsigned short      * Raw = 0;

/***********************************************************************
*                            ASCII_TO_UNICODE                          *
*   This assumes you are in ascii mode and want to convert an ascii    *
*   string into a unicode string. The unicode buffer is assumed to be  *
*   twice as long as the ascii buffer.                                 *
*                                                                      *
*   It is ok for the dest and sorc to be the same address as I copy    *
*   the last byte first.                                               *
***********************************************************************/
bool ascii_to_unicode( char * dest, char * sorc, unsigned int slen )
{
unsigned int dlen;

if ( !dest )
    return false;

if ( !sorc )
    return false;

dlen = slen * 2;

dest += dlen;
sorc += slen;

while ( slen )
    {
    dest--;
    *dest = NullChar;
    dest--;
    sorc--;
    *dest = *sorc;
    slen--;
    }

return true;
}

/***********************************************************************
*                         KILL_RAW_DATA_ARRAY                          *
***********************************************************************/
static void kill_raw_data_array()
{
if ( Raw )
    {
    delete[] Raw;
    Raw = 0;
    }
}

/***********************************************************************
*                            WRITE_PROFILE                             *
***********************************************************************/
static bool write_profile( const TCHAR * filename )
{

int     i;
HANDLE  fh;
DWORD   access_mode;
DWORD   share_mode;
DWORD   bytes_written;
DWORD   bytes_to_write;
BOOL    status;

/*
--------------------------------------------------
Copy the profile header to the wide profile header
-------------------------------------------------- */
ascii_to_unicode( Wph.machine_name, Ph.machine_name, MACHINE_NAME_LEN+1 );
ascii_to_unicode( Wph.part_name,    Ph.part_name,    PART_NAME_LEN+1 );

/*
-----------------------------------
Copy the rest of the profile header
----------------------------------- */
Wph.shot_number  = Ph.shot_number;
Wph.time_of_shot = Ph.time_of_shot;
Wph.n            = Ph.n;
Wph.last_pos_based_point = Ph.last_pos_based_point;
for ( i=0; i<MAX_CURVES; i++ )
    {
    Wph.curves[i].type  = Ph.curves[i].type;
    Wph.curves[i].max   = Ph.curves[i].max;
    Wph.curves[i].min   = Ph.curves[i].min;
    Wph.curves[i].units = Ph.curves[i].units;
    }

status      = FALSE;
access_mode = GENERIC_WRITE;
share_mode  = 0;

fh = CreateFile( filename, access_mode, share_mode, 0, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( fh != INVALID_HANDLE_VALUE )
    {
    status = WriteFile( fh, &Wph, sizeof(WIDE_PROFILE_HEADER), &bytes_written, 0 );
    if ( status )
        status = WriteFile( fh, &NofParms, sizeof(short), &bytes_written, 0 );

    if ( status && NofParms > 0 )
        {
        bytes_to_write = (DWORD) NofParms;
        bytes_to_write *= sizeof( float );
        status = WriteFile( fh, Parms, bytes_to_write, &bytes_written, 0 );
        }

    if ( status )
        {
        bytes_to_write = NofPoints * sizeof( unsigned short );
        status = WriteFile( fh, Raw, bytes_to_write, &bytes_written, 0 );
        }

    CloseHandle( fh );
    }

kill_raw_data_array();
if ( status )
    return true;

return false;
}

/***********************************************************************
*                             LOAD_PROFILE                             *
***********************************************************************/
static bool load_profile( const TCHAR * fname )
{

HANDLE  fh;
DWORD   access_mode;
DWORD   share_mode;
DWORD   bytes_read;
DWORD   bytes_to_read;
BOOL    status;
int     i;

kill_raw_data_array();
status      = FALSE;
access_mode = GENERIC_READ;
share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;

fh = CreateFile( fname, access_mode, share_mode, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( fh == INVALID_HANDLE_VALUE )
    return false;

status = ReadFile( fh, &Ph, sizeof(Ph), &bytes_read, 0 );

if ( status )
    status = ReadFile( fh, &NofParms, sizeof(short), &bytes_read, 0 );

bytes_to_read = (DWORD) NofParms;
bytes_to_read *= sizeof( float );

if ( status && NofParms > 0 )
    status = ReadFile( fh, Parms, bytes_to_read, &bytes_read, 0 );

NofPoints = 0;

if ( status )
    {
    status = FALSE;

    /*
    --------------------------------
    Count the total number of points
    -------------------------------- */
    for ( i=0; i<MAX_CURVES; i++ )
        {
        if ( Ph.curves[i].type == NO_CURVE_TYPE )
            break;
        NofPoints++;
        }

    if ( NofPoints > 0 )
        {

        NofPoints *= (DWORD) Ph.n;
        Raw = new unsigned short[NofPoints];
        if ( Raw )
            status = TRUE;
        }
    }

if ( status )
    {
    bytes_to_read = NofPoints * sizeof( unsigned short );
    status = ReadFile( fh, Raw, bytes_to_read, &bytes_read, 0 );
    }

CloseHandle( fh );

if ( status )
    return true;

kill_raw_data_array();
return false;
}

/***********************************************************************
*                          PROFILE_TO_UNICODE                          *
***********************************************************************/
static bool profile_to_unicode( const TCHAR * filename )
{

if ( load_profile(filename) )
    {
    write_profile(filename);
    return true;
    }

return false;
}

/***********************************************************************
*                                TOUNI                                 *
***********************************************************************/
bool touni( const TCHAR * file_to_convert )
{
DWORD   access_mode;
DWORD   share_mode;
DWORD   file_size;
DWORD   new_file_size;
DWORD   bytes_read;
DWORD   bytes_written;
TCHAR * buf;
HANDLE  fh;

share_mode  = FILE_SHARE_READ | FILE_SHARE_WRITE;
access_mode = GENERIC_READ;

fh = CreateFile( file_to_convert, access_mode, share_mode, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
if ( fh == INVALID_HANDLE_VALUE )
    return false;

file_size = GetFileSize( fh, 0 );
if ( file_size == 0 )
    {
    CloseHandle( fh );
    return true;
    }

new_file_size = file_size * 2;

buf = new TCHAR[new_file_size];
if ( !buf )
    {
    CloseHandle( fh );
    return false;
    }

if ( ReadFile( fh, buf, file_size, &bytes_read, 0)  )
    {
    ascii_to_unicode( buf, buf, file_size );

    CloseHandle( fh );

    share_mode  = 0;
    access_mode = GENERIC_WRITE;

    fh = CreateFile( file_to_convert, access_mode, share_mode, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
    if ( fh == INVALID_HANDLE_VALUE )
        return false;

    WriteFile( fh, buf, new_file_size, &bytes_written, 0 );
    CloseHandle( fh );
    fh = INVALID_HANDLE_VALUE;
    if ( new_file_size == bytes_written )
        return true;
    }
else
    {
    CloseHandle( fh );
    }

delete[] buf;
return false;
}

/***********************************************************************
*                               DO_FILE                                *
***********************************************************************/
void do_file( const TCHAR * filename )
{
static TCHAR * textfiles[] = { "TXT", "MSG", "PRG", "DAT", "SAV", "CSV" };
int   nof_textfiles = sizeof(textfiles)/sizeof(TCHAR *);

static TCHAR * profiles[]  = { "PRO", "MAS" };
int   nof_profiles = sizeof(profiles)/sizeof(TCHAR *);

TCHAR s[4];
const TCHAR * cp;
int i;
int slen;

slen = lstrlen( filename );
cp = filename + slen - 1;

slen = 0;
while ( slen <= 4 )
    {
    if ( *cp == PeriodChar )
        break;
    cp--;
    slen++;
    }

if ( *cp != PeriodChar )
    return;

cp++;
lstrcpy( s, cp );

for ( i=0; i<nof_textfiles; i++ )
    {
    if ( strings_are_equal(s, textfiles[i]) )
        {
        cprintf( "\n\rConverting text file %s ", filename );
        touni( filename );
        return;
        }
    }

for ( i=0; i<nof_profiles; i++ )
    {
    if ( strings_are_equal(s, profiles[i]) )
        {
        cprintf( "\n\rConverting profile %s ", filename );
        profile_to_unicode( filename );
        return;
        }
    }

}

/***********************************************************************
 *                            GET_DIR_LIST                             *
***********************************************************************/
void get_dir_list( const TCHAR * rootdir )
{
TCHAR s[MAX_PATH+1];
TCHAR s1[MAX_PATH+1];
HANDLE fh;
WIN32_FIND_DATA fdata;

lstrcpy( s, rootdir );
append_filename_to_path( s, StarDotStar );

fh = FindFirstFile( s, &fdata );
if ( fh != INVALID_HANDLE_VALUE )
    {
    while ( TRUE )
        {
        if ( (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
            {
            if ( fdata.cFileName[0] != PeriodChar )
                {
                lstrcpy( s1, rootdir );
                append_filename_to_path( s1, fdata.cFileName );
                get_dir_list( s1 );
                }
            }
        else
            {
            lstrcpy( s1, rootdir );
            append_filename_to_path( s1, fdata.cFileName );
            do_file( s1 );
            }

        if ( !FindNextFile(fh, &fdata) )
            break;
        }

    FindClose( fh );
    }
}

/***********************************************************************
*                                 MAIN                                 *
*                                                                      *
* copy ascii \v5\backup\bup files and dirs to \v5wide\backup\bup\*.*   *
* cd \v5wide\backup\bup                                                *
* asctouni will convert all text files and profiles to unicode. you    *
* can then restore them                                                *
***********************************************************************/
int main( void )
{

get_dir_list( "." );

return 0;
}

