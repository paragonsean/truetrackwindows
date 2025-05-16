#include <windows.h>

#include "..\include\visiparm.h"

/***********************************************************************
*                           ADJUST_FOR_UNITS                           *
***********************************************************************/
double adjust_for_units( short dest_units_id, double x, short sorc_units_id )
{

const double IN_TO_CM   = 2.54;
const double IN_TO_MM   = 25.4;
const double IN2_TO_CM2 = IN_TO_CM * IN_TO_CM;
const double IN2_TO_MM2 = IN_TO_MM * IN_TO_MM;
const double IN3_TO_CM3 = IN_TO_CM * IN_TO_CM * IN_TO_CM;
const double IN3_TO_MM3 = IN_TO_MM * IN_TO_MM * IN_TO_MM;

const double CM_TO_MM   = 10.0;
const double CM2_TO_MM2 = CM_TO_MM * CM_TO_MM;
const double CM3_TO_MM3 = CM_TO_MM * CM_TO_MM * CM_TO_MM;

const double KG_TO_LBS   = 2.204622622;
const double LBS_TO_KG   = 0.4535923699689;

switch ( sorc_units_id )
    {
    case LBS_UNITS:
        switch ( dest_units_id )
            {
            case KG_UNITS:
                x *= LBS_TO_KG;
                break;
            }
        break;

    case KG_UNITS:
        switch ( dest_units_id )
            {
            case LBS_UNITS:
                x *= KG_TO_LBS;
                break;
            }
        break;

    case INCH_UNITS:
        switch ( dest_units_id )
            {
            case CM_UNITS:
                x *= IN_TO_CM;
                break;

            case MM_UNITS:
                x *= IN_TO_MM;
                break;

            case FEET_UNITS:
                x /= 12.0;
                break;
            }
        break;

    case CM_UNITS:
        switch ( dest_units_id )
            {
            case INCH_UNITS:
                x /= IN_TO_CM;
                break;

            case MM_UNITS:
                x *= CM_TO_MM;
                break;

            case FEET_UNITS:
                x /= 30.48;
                break;
            }
        break;

    case MM_UNITS:
        switch ( dest_units_id )
            {
            case INCH_UNITS:
                x /= IN_TO_MM;
                break;

            case CM_UNITS:
                x /= CM_TO_MM;
                break;

            case FEET_UNITS:
                x /= 304.8;
                break;
            }
        break;

    case FEET_UNITS:
        switch ( dest_units_id )
            {
            case INCH_UNITS:
                x *= 12.0;
                break;

            case CM_UNITS:
                x *= 30.48;
                break;

            case MM_UNITS:
                x *= 304.8;
                break;
            }
        break;

    case IN_SQUARED_UNITS:
        switch ( dest_units_id )
            {
            case CM_SQUARED_UNITS:
                x *= IN2_TO_CM2;
                break;

            case MM_SQUARED_UNITS:
                x *= IN2_TO_MM2;
                break;
            }
        break;

    case CM_SQUARED_UNITS:
        switch ( dest_units_id )
            {
            case IN_SQUARED_UNITS:
                x /= IN2_TO_CM2;
                break;

            case MM_SQUARED_UNITS:
                x *= CM2_TO_MM2;
                break;
            }
        break;

    case MM_SQUARED_UNITS:
        switch ( dest_units_id )
            {
            case IN_SQUARED_UNITS:
                x /= IN2_TO_MM2;
                break;

            case CM_SQUARED_UNITS:
                x /= CM2_TO_MM2;
                break;
            }
        break;

    case IN_CUBED_UNITS:
        switch ( dest_units_id )
            {
            case CM_CUBED_UNITS:
                x *= IN3_TO_CM3;
                break;

            case MM_CUBED_UNITS:
                x *= IN3_TO_MM3;
                break;
            }
        break;

    case CM_CUBED_UNITS:
        switch ( dest_units_id )
            {
            case IN_CUBED_UNITS:
                x /= IN3_TO_CM3;
                break;

            case MM_CUBED_UNITS:
                x *= CM3_TO_MM3;
                break;
            }
        break;

    case MM_CUBED_UNITS:
        switch ( dest_units_id )
            {
            case IN_CUBED_UNITS:
                x /= IN3_TO_MM3;
                break;

            case CM_CUBED_UNITS:
                x /= CM3_TO_MM3;
                break;
            }
        break;

    case IPS_UNITS:
        switch ( dest_units_id )
            {
            case FPM_UNITS:
                x *= 5.0;
                break;

            case CMPS_UNITS:
                x *= 2.54;
                break;

            case MPS_UNITS:
                x *= .0254;
                break;
            }
        break;

    case FPM_UNITS:
        switch ( dest_units_id )
            {
            case IPS_UNITS:
                x *= .2;
                break;

            case CMPS_UNITS:
                x *= .508;
                break;

            case MPS_UNITS:
                x *= .00508;
                break;
            }
        break;

    case CMPS_UNITS:
        switch ( dest_units_id )
            {
            case IPS_UNITS:
                x /= 2.54;
                break;

            case FPM_UNITS:
                x *= 1.9685;
                break;

            case MPS_UNITS:
                x *= .01;
                break;
            }
        break;

    case MPS_UNITS:
        switch ( dest_units_id )
            {
            case IPS_UNITS:
                x /= .0254;
                break;

            case FPM_UNITS:
                x *= 196.85;
                break;

            case CMPS_UNITS:
                x *= 100.0;
                break;
            }
        break;

    case KG_PER_CM2_UNITS:
        switch ( dest_units_id )
            {
            case BAR_UNITS:
                x *= .98066;
                break;

            case PSI_UNITS:
                x *= 14.2233;
                break;
            }
        break;

    case PSI_UNITS:
        switch ( dest_units_id )
            {
            case BAR_UNITS:
                x *= .0689467;
                break;

            case KG_PER_CM2_UNITS:
                x *= .070307;
                break;
            }
        break;

    case BAR_UNITS:
        switch ( dest_units_id )
            {
            case PSI_UNITS:
                x *= 14.5038;
                break;

            case KG_PER_CM2_UNITS:
                x *= 1.01972;
                break;
            }
        break;

    case DEG_F_UNITS:
        if ( dest_units_id == DEG_C_UNITS )
            {
            x -= 32.0;
            x *= 5.0;
            x /= 9.0;
            }
        break;

    case DEG_C_UNITS:
        if ( dest_units_id == DEG_C_UNITS )
            {
            x *= 9.0;
            x /= 5.0;
            x += 32.0;
            }
        break;

    case VOLTS_UNITS:
        break;

    case SECOND_UNITS:
        if ( dest_units_id == MS_UNITS )
            return x * 1000.0;
        break;

    case MS_UNITS:
        if ( dest_units_id == SECOND_UNITS )
            return x / 1000.0;
        break;

    case IN_LB_UNITS:
        if ( dest_units_id == KJ_UNITS )
            return x * 0.000113;
        break;

    case KJ_UNITS:
        if ( dest_units_id == IN_LB_UNITS )
            return x * 8850.0;
        break;
    }

return x;
}

/***********************************************************************
*                           ADJUST_FOR_UNITS                           *
***********************************************************************/
float adjust_for_units( short dest_units_id, float sorc, short sorc_units_id )
{
double x;
x = (double) sorc;
return (float) adjust_for_units( dest_units_id, x, sorc_units_id );
}

/***********************************************************************
*                              DUPS_TO_VU                              *
*                                                                      *
*          Convert distance units per second to velocity units         *
***********************************************************************/
double dups_to_vu( short velocity_units_id, short distance_units_id )
{

double * p;
double  x;

                   /*   IPS      FPM        CMPS   MPS */
static double ic[] = {  1.0,      5.0,      2.54,  .0254 };
static double ft[] = { 12.0,     60.0,     30.48,  .3048 };
static double mm[] = {   .03937,  0.1969,   0.10,  .0010 };
static double cm[] = {   .3937,   2.2926,   1.0,   .0100 };

x = 1.0;

switch ( distance_units_id )
    {
    case INCH_UNITS:
        p = ic;
        break;

    case FEET_UNITS:
        p = ft;
        break;

    case CM_UNITS:
        p = cm;
        break;

    case MM_UNITS:
        p = mm;
        break;

    default:
        p = 0;
    }

if ( p )
    {
    switch ( velocity_units_id )
        {
        case IPS_UNITS:
            x = p[0];
            break;

        case FPM_UNITS:
            x = p[1];
            break;

        case CMPS_UNITS:
            x = p[2];
            break;

        case MPS_UNITS:
            x = p[3];
            break;
        }
    }

return x;
}

