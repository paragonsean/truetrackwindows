#ifndef _ALARM_SUMMARY_CLASS_
#define _ALARM_SUMMARY_CLASS_

const int32 TOTALS_PARAMETER_NUMBER       = NO_PARAMETER_NUMBER;
const int32 TOTAL_COUNT_ALARMTYPE         = WARNING_MIN;
const int32 TOTAL_ALARM_COUNT_ALARMTYPE   = ALARM_MIN;
const int32 TOTAL_WARNING_COUNT_ALARMTYPE = WARNING_MAX;

#ifndef _STRUCTS_
#include "..\include\structs.h"
#endif

#ifndef _GENLIST_H
#include "..\include\genlist.h"
#endif

#ifndef _VISI_PARAM_H_
#include "..\include\param.h"
#endif

#ifndef _TIME_CLASS_
#include "..\include\timeclas.h"
#endif

struct ALARM_COUNTS_ENTRY
    {
    int32 parameter_number;
    int32 counts[NOF_ALARM_LIMIT_TYPES];
    };

class ALARM_SUMMARY_CLASS
    {
    private:

    PART_NAME_ENTRY    pe;
    GENERIC_LIST_CLASS alarmlist;
    int32              total_shots;
    int32              total_alarms_count;
    int32              total_warnings_count;
    TIME_CLASS         start;
    TIME_CLASS         end;
    int32              start_shift;
    int32              end_shift;

    public:

    ALARM_SUMMARY_CLASS();
    ~ALARM_SUMMARY_CLASS();

    ALARM_COUNTS_ENTRY * add( int32 new_parameter_number );
    int32                count( int32 parameter_number, int32 alarm_type );
    void                 empty( void );
    int32                end_shift_number( void ) { return end_shift; }
    ALARM_COUNTS_ENTRY * find( int32 parameter_number_to_find );
    BOOLEAN              get_counts( void );
    BOOLEAN              get_counts( SYSTEMTIME & date_to_find );
    BOOLEAN              get_counts( SYSTEMTIME & date_to_find, int32 shift_to_find );
    BOOLEAN              get_counts( SYSTEMTIME & date_to_find, int32 shift_to_find, TCHAR * fname );
    void                 set_counts( float * parameter, PARAMETER_CLASS * p );
    void                 save( const SYSTEMTIME & date_to_save, int32 shift_to_save, BOOLEAN is_update, TCHAR * fname );
    void                 save( const SYSTEMTIME & date_to_save, int32 shift_to_save, BOOLEAN is_update );
    void                 save( const SYSTEMTIME & date_to_save, int32 shift_to_save );
    void                 set_part( TCHAR * new_computer, TCHAR * new_machine,  TCHAR * new_part );
    void                 set_part( PART_NAME_ENTRY & new_pe ) { pe = new_pe; }
    int32                start_shift_number( void ) { return start_shift; }
    TIME_CLASS         & end_time( void ) { return end; }
    TIME_CLASS         & start_time( void ) { return start; }

    int32                total_shot_count( void )    { return total_shots;          }
    int32                total_alarm_count( void )   { return total_alarms_count;   }
    int32                total_warning_count( void ) { return total_warnings_count; }
    };

#endif
