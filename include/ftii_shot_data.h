#ifndef _FTII_SHOT_DATA_
#define _FTII_SHOT_DATA_

const int ASCII_TYPE        =  0;
const int SAMPLE_TYPE       =  1;
const int PARAMETERS_TYPE   =  2;
const int OP_STATUS_TYPE    =  3;
const int OSCILLOSCOPE_TYPE =  4;
const int SOCKET_ERROR_TYPE =  5;

const int NOF_STATIC_ANALOGS       = 8;
const int NOF_FTII_FILE_PARAMETERS = 11;
const int NOF_FTII_FILE_CURVES     = 10; /* This is the number of curves that have max and min data in curvedata */

const int FIRST_ANALOG_CURVE_INDEX = 2;  /* FTII_FILE_DATA */

/*
-----------------------------------------------------------------------------------------
Position of the first static analog in the FTII_PARAMETER_ENTRY array in the FTII_PROFILE
----------------------------------------------------------------------------------------- */
const int FIRST_STATIC_ANALOG_INDEX =   3;

struct EXTENDED_ANALOG_ENTRY
    {
    short parameter_number;
    unsigned short value;
    EXTENDED_ANALOG_ENTRY() { parameter_number = NO_PARAMETER_NUMBER; value = 0; }
    void operator=(EXTENDED_ANALOG_ENTRY & sorc) { parameter_number=sorc.parameter_number; value=sorc.value; }
    };

struct NEW_FTII_DATA {
    SOCKET sorc;
    int    type;
    void * buf;
    int    len;
    };

/*
----------------------------------
FasTrak II sample point : 56 bytes
---------------------------------- */
struct FTII_SAMPLE {
    unsigned short analog[NOF_FTII_ANALOGS];
    unsigned       velcount[X4_COUNTS_PER_MARK];
    unsigned       isw1;
    unsigned       isw4;
    unsigned       osw1;
    unsigned       one_ms_timer;
    int            position;
    int            sample_num;
    };

struct FTII_OP_STATUS_DATA {
    int            dac[4]; // DAC voltage * 1000
    unsigned short analog[20];
    int            pos;
    int            vel;
    unsigned       isw1;
    unsigned       isw4;
    unsigned       osw1;
    unsigned       monitor_status;
    unsigned       status_word1;
    unsigned       config_word1;
    int            warning;
    int            fatal_error;
    int            blk_no; // Current control program step
    FTII_OP_STATUS_DATA();
    bool operator==(const FTII_OP_STATUS_DATA & sorc );
    bool operator!=(const FTII_OP_STATUS_DATA & sorc ) { return !operator==(sorc); }
    void operator=( const FTII_OP_STATUS_DATA & sorc );
    };

struct FTII_OSCILLOSCOPE_DATA {
    short ch_17_20[2000][4];
    };

struct VERSION_1_FTII_PARAMETERS
	{
    unsigned       cycle_time;   // mS
    unsigned short biscuit_size; // X4 counts
    short          eos_pos;      // X1 counts
    };

/*
-------------------------------
FasTrak Computed Shot Paramters
------------------------------- */
struct FTII_PARAMETERS
	{
    unsigned       cycle_time;   // mS
    unsigned short biscuit_size; // X4 counts
    short          eos_pos;      // X1 counts
    unsigned short static_analog[NOF_STATIC_ANALOGS];
    void operator=( const FTII_PARAMETERS & sorc );
    void operator=( const VERSION_1_FTII_PARAMETERS & sorc );
    };

class FTII_POSITION_DATA {
    public:
    unsigned       velcount[X4_COUNTS_PER_MARK];
    unsigned       isw1;
    unsigned       osw1;
    unsigned short isw4;
    short          position;
    unsigned short analog[NOF_FTII_ANALOGS];

    void operator=( const FTII_SAMPLE & sorc );
    };

class FTII_TIME_DATA {
    public:
    unsigned       isw1;
    unsigned       osw1;
    unsigned short isw4;
    short          position;
    unsigned short analog[NOF_FTII_ANALOGS];

    void operator=( const FTII_SAMPLE & sorc );
    };

class FTII_NET_DATA {
    public:

    int      timer_frequency;
    int      np;
    int      nt;
    int      us_per_time_sample;
    int      internal_shot_number;
    FILETIME shot_time;

    FTII_SAMPLE     * position_sample;
    FTII_SAMPLE     * time_sample;
    FTII_PARAMETERS   parameter;

    FTII_NET_DATA();
    ~FTII_NET_DATA();
    void clear();

    void set_shot_time( FILETIME & t );
    void set_time_based_points( FTII_SAMPLE * p, int nof_samples );
    void set_time_based_points( NEW_FTII_DATA & nd );
    void set_position_based_points( FTII_SAMPLE * p, int nof_samples );
    void set_position_based_points( NEW_FTII_DATA & nd );
    void set_parameters( NEW_FTII_DATA & nd );
    void set_parameters( FTII_PARAMETERS * p );
    void set_timer_frequency( int new_timer_frequency ) { timer_frequency = new_timer_frequency; }
    void set_us_per_time_sample( int new_us ) { us_per_time_sample = new_us; }

    BOOLEAN write_csv( STRING_CLASS & dest_file_path );
    };

class FTII_FILE_DATA {
    public:
    int      channel_mode;
    int      timer_frequency;
    int      np;
    int      nt;
    int      cs_to_point_1_ms;
    int      us_per_time_sample;
    int      internal_shot_number;
    FILETIME shot_time;

    FTII_MIN_MAX_DATA    curvedata[NOF_FTII_FILE_CURVES];
    FTII_POSITION_DATA * position_sample;
    FTII_TIME_DATA     * time_sample;
    int                  parameter[NOF_FTII_FILE_PARAMETERS];

    int                  nof_extended_analogs;  // Added in file version 3
    EXTENDED_ANALOG_ENTRY * extended_analog;

    FTII_FILE_DATA();
    ~FTII_FILE_DATA();
    void    clear();
    void    move( FTII_FILE_DATA & sorc );
    void    operator=( const FTII_NET_DATA & sorc );
    void    set_nof_extended_analogs( int n );
    BOOLEAN get( STRING_CLASS & sorc_file_path );
    BOOLEAN put( STRING_CLASS & dest_file_path );
    BOOLEAN write_csv( STRING_CLASS & dest_file_path );
    };

class FTII_ARRAY_DATA {
    public:
    unsigned timer_frequency;
    int      last_position_based_point;
    int      np;
    int      nt;
    int      n;

    int      * position;
    int      * velocity;
    unsigned * timer;
    unsigned * isw1;
    unsigned * isw4;
    unsigned * osw1;
    unsigned * analog[NOF_FTII_ANALOGS];
    int        channel[NOF_FTII_ANALOGS];
    FTII_ARRAY_DATA();
    virtual ~FTII_ARRAY_DATA();
    void     clear();
    void     operator=( const FTII_FILE_DATA & sorc );
    double   interpolated_velocity( double p );
    int      index_from_position( double p );
    int      index_from_time( double ms_past_impact );
    unsigned interpolated_timer_count( double p );
    unsigned timer_counts_between_positions( double p1, double p2 ) { return(interpolated_timer_count(p2) - interpolated_timer_count(p1)); }
    double   velocity_between_positions( double p1, double p2 );
    void write_csv( TCHAR * destfile  );
    };

class FTII_PARAMETER_ENTRY {
    public:
    int index;
    union { int ivalue; unsigned uvalue; };
    FTII_PARAMETER_ENTRY() { index=NO_PARAMETER_INDEX; uvalue=0; }
    ~FTII_PARAMETER_ENTRY() {}
    };

class FTII_PROFILE : public FTII_ARRAY_DATA {
    public:

    int      us_per_time_sample;
    int      nof_analogs;
    int      nof_curves;
    FILETIME shot_time;

    FTII_MIN_MAX_DATA    * curvedata;
    FTII_PARAMETER_ENTRY * parameter;

    int                  nof_extended_analogs;  // Added in file version 3
    EXTENDED_ANALOG_ENTRY * extended_analog;

    void clear();

    public:
    FTII_PROFILE();
    ~FTII_PROFILE();
    void     operator=( const FTII_FILE_DATA & sorc );
    int      curve_index( int curve_type );
    BOOLEAN  get( STRING_CLASS & sorc_file_path );
    double   ms( int timer_index );
    double   interpolated_ms( double timer_index );
    double   ms_between_positions( double p1, double p2 );
    double   interpolated_postion_from_velocity( int target_velocity, int i1, int i2 );
    void     write_to_csv( TCHAR * filename );
    };

#endif
