/*
---------------------------------------------------------------------------------
                                     MSC 6.0
cl /nologo /GX /MT /W3 /Zp1 /O1 /D "WIN32" /D "_CONSOLE" /D "NDEBUG" protocsv.cpp
--------------------------------------------------------------------------------- */

#include <windows.h>
#include <stdio.h>
#include <conio.h>

typedef unsigned short BITSETYPE;
typedef long int       int32;

const short MACHINE_NAME_LEN      = 3;
const short PART_NAME_LEN         = 8;
const short MAX_CURVES            = 8;
const short MAX_POINTS            = 5000;
const short MAX_PARMS             = 60;

/*
-----------------------------------------------------
These are the curve types in the CURVE_DATA structure
----------------------------------------------------- */
const BITSETYPE NO_CURVE_TYPE       = 0;
const BITSETYPE TIME_CURVE          = 0x0001;
const BITSETYPE POSITION_CURVE      = 0x0002;
const BITSETYPE VELOCITY_CURVE      = 0x0004;
const BITSETYPE ROD_PRESSURE_CURVE  = 0x0008;
const BITSETYPE HEAD_PRESSURE_CURVE = 0x0010;
const BITSETYPE CHAN_1_ANALOG_CURVE = 0x0020;
const BITSETYPE CHAN_2_ANALOG_CURVE = 0x0040;
const BITSETYPE CHAN_3_ANALOG_CURVE = 0x0080;
const BITSETYPE CHAN_4_ANALOG_CURVE = 0x0100;
const BITSETYPE DIFF_PRESSURE_CURVE = 0x0200;
const BITSETYPE CHAN_5_ANALOG_CURVE = 0x0400;
const BITSETYPE CHAN_6_ANALOG_CURVE = 0x0800;
const BITSETYPE CHAN_7_ANALOG_CURVE = 0x1000;
const BITSETYPE CHAN_8_ANALOG_CURVE = 0x2000;

const DWORD  AccessMode = GENERIC_READ;
const DWORD  ShareMode  = FILE_SHARE_READ | FILE_SHARE_WRITE;

const double MaxFloatShort = float( 0xFFFF );

TCHAR * UnitName[] = {
  {"None"},
  {"Inches"},
  {"CM"},
  {"Feet"},
  {"Seconds"},
  {"IPS"},
  {"FPM"},
  {"CMPS"},
  {"PSI"},
  {"BAR"},
  {"MM"},
  {"MPS"},
  {"DEG_F"},
  {"DEG_C"},
  {"VOLTS"},
  {"KG_PER_CM2"},
  {"MS"},
  {"IN_LB"},
  {"KJ"},
  {"LBS"},
  {"KG"},
  {"IN_CUBED"},
  {"CM_CUBED"},
  {"MM_CUBED"},
  {"IN_SQUARED"},
  {"CM_SQUARED"},
  {"MM_SQUARED"},
  {"LBS_PER_IN_CUBED"},
  {"G_PER_CM_CUBED"},
  {"INCH-HG"},
  {"DIGITAL_INPUT"},
  {"MEGA_PASCAL"},
  {"mBAR"}
  };

struct CURVE_DATA
    {
    BITSETYPE type;
    float     max;
    float     min;
    short     units;
    };

struct PROFILE_HEADER
    {
    TCHAR      machine_name[MACHINE_NAME_LEN+1];        /* 4 bytes or 4 unicode words */
    TCHAR      part_name[PART_NAME_LEN+1];              /* 9 bytes or 9 unicode words */
    int32      shot_number;
    FILETIME   time_of_shot;      /* 8 bytes */
    short      n;
    short      last_pos_based_point; /* NofPosBasedPoints - 1 */
    CURVE_DATA curves[MAX_CURVES];
    };

/***********************************************************************
*                                 MAIN                                 *
***********************************************************************/
int main( void )
{
TCHAR   buf[103];
TCHAR   fname[120];
TCHAR * cp;

PROFILE_HEADER Ph;

unsigned short raw[MAX_POINTS];

float    parameters[MAX_PARMS];
double   points[MAX_CURVES][MAX_POINTS];
unsigned short rp[MAX_CURVES][MAX_POINTS];

short    i;
short    j;
short    nof_parameters;
short    nof_curves;
double   b;
double   ymin;
DWORD    bytes_read;
DWORD    bytes_to_read;
HANDLE   fh;
FILE   * fp;

cprintf( "\n\rEnter a File Name" );
cprintf( "\n\re.g. if the file is 00100232.PRO then enter 00100232" );

while ( TRUE )
    {
    cprintf( "\n\r File Name? " );
    buf[0] = 100;
    cp = cgets( buf );
    if ( buf[1] < 1 )
        break;

    lstrcpy( fname, cp );
    lstrcat( fname, TEXT(".pro") );

    fh = CreateFile( fname, AccessMode, ShareMode, 0, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0 );
    if ( fh == INVALID_HANDLE_VALUE )
        {
        cprintf( "\n\r File not found." );
        }
    else
        {
        ReadFile( fh, &Ph, sizeof(Ph), &bytes_read, 0 );
        ReadFile( fh, &nof_parameters, sizeof(short), &bytes_read, 0 );

        /*
        ---------------------------------------------------------------------------------
        The 0th parameter is the first one in the parmlist.txt file in the part directory
        --------------------------------------------------------------------------------- */
        if ( nof_parameters > 0 )
            ReadFile( fh, parameters, nof_parameters*sizeof(float), &bytes_read, 0 );

        nof_curves = 0;
        bytes_to_read = DWORD(Ph.n) * sizeof( unsigned short );
        for ( j=0; j<MAX_CURVES; j++ )
            {
            /*
            --------------------------------------------------
            A curve with no curve type implies this is the end
            -------------------------------------------------- */
            if ( Ph.curves[j].type == NO_CURVE_TYPE )
                break;

            ymin = Ph.curves[j].min;
            b    = Ph.curves[j].max - ymin;
            b    /= MaxFloatShort;

            ReadFile( fh, raw, bytes_to_read, &bytes_read, 0 );
            for ( i=0; i<Ph.n; i++ )
                {
                points[j][i] = double( raw[i] ) * b + ymin;
                rp[j][i] = raw[i];
                }

            nof_curves++;
            }
        CloseHandle( fh );

        if ( nof_curves > 0 )
            {
            lstrcpy( fname, cp );
            lstrcat( fname, TEXT(".csv") );

            fp = fopen( fname, "w" );
            if ( fp != NULL )
                {
                /*
                --------------------
                Write the parameters
                for ( i=0; i<nof_parameters; i++ )
                    {
                    fprintf( fp, "%3d %f\n", (int) i, parameters[i] );
                    }
                -------------------- */

                fprintf( fp, "Machine %s,Part %s, Shot Number %i, Number of Position Points %i, Number of Points %i", Ph.machine_name, Ph.part_name, Ph.shot_number, Ph.last_pos_based_point+1, Ph.n );
                fprintf( fp, "\n" );

                /*
                --------------------------------------------------------
                The first line is a list of the curve types.
                Normally the first column is the position curve (type 2)
                and column 2 is the time curve (type 1).
                A type of 0x30 indicates the curve is Head Pressure
                (type 0x10) from channel 1 analog input (type 0x20);
                The older versions stored a differential curve
                (type 0x200). This is now calculated at display time.
                -------------------------------------------------------- */
                for ( j=0; j<nof_curves; j++ )
                    {
                    fprintf( fp, "0x%04X", (int) Ph.curves[j].type );
                    if ( j < (nof_curves - 1) )
                        fprintf( fp, "," );
                    }
                fprintf( fp, "\n" );

                for ( j=0; j<nof_curves; j++ )
                    {
                    fprintf( fp, "%f", Ph.curves[j].min );
                    if ( j < (nof_curves - 1) )
                        fprintf( fp, "," );
                    }
                fprintf( fp, "\n" );

                for ( j=0; j<nof_curves; j++ )
                    {
                    fprintf( fp, "%f", Ph.curves[j].max );
                    if ( j < (nof_curves - 1) )
                        fprintf( fp, "," );
                    }
                fprintf( fp, "\n" );

                for ( j=0; j<nof_curves; j++ )
                    {
                    i = (int) Ph.curves[j].units;
                    fprintf( fp, "%s", UnitName[i] );
                    if ( j < (nof_curves - 1) )
                        fprintf( fp, "," );
                    }
                fprintf( fp, "\n" );

                for ( i=0; i<Ph.n; i++ )
                    {
                    for ( j=0; j<nof_curves; j++ )
                        {
                        fprintf( fp, "%9.4f", points[j][i] );
                        /* fprintf( fp, "0x%04X", rp[j][i]        ); */
                        if ( j < (nof_curves - 1) )
                            fprintf( fp, "," );
                        }
                    fprintf( fp, "\n" );
                    }
                fclose( fp );
                }
            else
                {
                cprintf( "\n\r Could not open new file [%s].", fname );
                }
            }
        else
            {
            cprintf( "\n\r No Curves" );
            }

        }

    }

return 0;
}

