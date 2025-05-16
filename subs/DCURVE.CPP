#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\subs.h"
#include "..\include\names.h"
#include "..\include\verrors.h"

struct DCURVE_ENTRY
    {
    int number;
    double head_coefficient;
    double rod_coefficient;
    };

static int            NofDCurves             = 0;
static DCURVE_ENTRY * DCurve                 = 0;

/***********************************************************************
*                           CLEANUP_DCURVES                            *
***********************************************************************/
static void cleanup_dcurves( void )
{

if ( DCurve )
    {
    delete[] DCurve;
    DCurve     = 0;
    NofDCurves = 0;
    }

}

/***********************************************************************
*                            RELOAD_DCURVES                            *
***********************************************************************/
static BOOLEAN reload_dcurves( void )
{

int      i;
DB_TABLE t;

cleanup_dcurves();

if ( t.open( diffcurve_dbname(), DCURVE_RECLEN, PFL ) )
    {
    NofDCurves = t.nof_recs();
    if ( NofDCurves > 0 )
        {
        DCurve = new DCURVE_ENTRY[NofDCurves];
        if ( !DCurve )
            {
            NofDCurves = 0;
            error_message( FILL_DCURVES_SUB, NO_MEM_ALLOC_ERROR );
            return FALSE;
            }
        }

    if ( NofDCurves > 0 )
        {
        i = 0;
        while ( t.get_next_record(FALSE) && i < NofDCurves )
            {
            DCurve[i].number           = t.get_long(   DCURVE_CURVE_NUMBER_OFFSET );
            DCurve[i].head_coefficient = t.get_double( DCURVE_HEAD_PRES_OFFSET    );
            DCurve[i].rod_coefficient  = t.get_double( DCURVE_ROD_PRES_OFFSET     );
            i++;
            }
        }
    t.close();
    }

return TRUE;
}

/***********************************************************************
*                            DCURVE_STARTUP                            *
***********************************************************************/
BOOLEAN dcurve_startup( void )
{
return reload_dcurves();
}

/***********************************************************************
*                           DCURVE_SHUTDOWN                            *
***********************************************************************/
void dcurve_shutdown( void )
{
cleanup_dcurves();
}

/***********************************************************************
*                       GET_DCURVE_COEFFICIENTS                        *
***********************************************************************/
BOOLEAN get_dcurve_coefficients( float & dest_head_coef, float & dest_rod_coef, short curve_number )
{
int i;

for ( i=0; i<NofDCurves; i++ )
    {
    if ( DCurve[i].number == int(curve_number) )
        {
        dest_head_coef = float( DCurve[i].head_coefficient );
        dest_rod_coef  = float( DCurve[i].rod_coefficient  );
        return TRUE;
        }
    }

dest_head_coef = 1.0;
dest_rod_coef  = 1.0;
return FALSE;
}

/***********************************************************************
*                       GET_DCURVE_COEFFICIENTS                        *
***********************************************************************/
BOOLEAN get_dcurve_coefficients( double & dest_head_coef, double & dest_rod_coef, int curve_number )
{
int i;

for ( i=0; i<NofDCurves; i++ )
    {
    if ( DCurve[i].number == curve_number )
        {
        dest_head_coef = DCurve[i].head_coefficient;
        dest_rod_coef  = DCurve[i].rod_coefficient;
        return TRUE;
        }
    }

dest_head_coef = 1.0;
dest_rod_coef  = 1.0;
return FALSE;
}

