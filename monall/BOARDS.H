#ifndef _BOARD_CLASS_
#define _BOARD_CLASS_

#ifndef _DOWN_DATA_CLASS_
#include "..\include\evclass.h"
#endif

#define NO_BOARD               -1
#define NO_BOARD_STATUS         0
#define NO_BOARD_ADDRESS        0

#define NO_BOARD_STATE          0
#define MONITORING_STATE        1
#define NOT_INITIALIZED_STATE   2
#define SINGLE_SCAN_STATE       4

#define MAX_BOARDS   8

BITSETYPE wirebit( short wire_number );  /* Defined in boards.cpp */

struct MUX_ENTRY
    {
    TCHAR      machine[MACHINE_NAME_LEN+1];
    TCHAR      part[PART_NAME_LEN+1];
    int        down_state;
    long       cycle_timeout_seconds;
    long       time_of_last_shot;
    short      dist_units;
    short      vel_units;
    short      ms_per_sample;
    ANALOG_SENSOR_SETUP_DATA * asensor[MAX_FT_CHANNELS];
    POS_SENSOR_SETUP_DATA    * psensor;
    };

struct HARDWARE_BOARD_ENTRY
    {
    BOARD_DATA     address;
    BITSETYPE      type;
    short          mhz;
    BOARD_DATA     status;
    BITSETYPE      state;
    BITSETYPE      dout_bits;
    BOOLEAN        need_dout_update;
    short          cycles_per_time_count;
    short          counts_per_pos_mark;
    short          current_mux;
    MUX_ENTRY      mux[MAX_MUX_CHANNELS];
    BOOLEAN        monitoring;
    void           clearwire( short wire_number );
    void           setwire( short wire_number );
    };

class BOARD_CLASS
    {
    private:

    short   currentboard;
    short   machine_list_board;
    short   machine_list_mux;
    BOOLEAN init_one_channel( short x );

    public:

    BOARD_CLASS( void ) { currentboard = NO_BOARD; machine_list_board = NO_BOARD; machine_list_mux = NO_MUX_CHANNEL; }
    BOARD_DATA  address( void );
    HARDWARE_BOARD_ENTRY * board( void );
    MUX_ENTRY            * current_mux_entry( void );
    BOOLEAN     command_mode( void );
    short       current_mux_channel( void );
    BOOLEAN     data_collection_mode( void );
    BOOLEAN     find( short board_number );
    short       index( void ) { return currentboard; }
    BOOLEAN     init_one_board( void );
    BOOLEAN     init_all_boards( void );
    BOOLEAN     locate( short & board_number, short & channel_number, TCHAR * machine_to_locate );
    BOOLEAN     monitoring( void );
    BOOLEAN     next( void );
    short       number( void );
    TCHAR     * next_machine( void );
    void        rewind_machine_list( void ) { machine_list_board = NO_BOARD; machine_list_mux = NO_MUX_CHANNEL; }
    short       read_current_mux_channel( void );
    void        refresh_downtime_state( DOWN_DATA_CLASS & dd );
    void        set_machine( short mux_channel, TCHAR * machine_name, TCHAR * part_name );
    void        set_machine( short mux_channel, TCHAR * machine_name, TCHAR * part_name, long sec_until_down );
    BOOLEAN     set_monitoring( BOOLEAN new_monitoring_state );
    void        set_port_addresses( void );
    BOOLEAN     set_pos_sensor_pointer( void );
    void        set_status( BOARD_DATA new_status );
    void        shutdown( void );
    BOOLEAN     startup( void );
    BOOLEAN     start_all_boards( void );
    BOOLEAN     start_one_board( void );
    BOOLEAN     stop_all_boards( void );
    BOARD_DATA  status( void );
    };

#endif

