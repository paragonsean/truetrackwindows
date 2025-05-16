#ifndef _STRUCTS_

#define _STRUCTS_

struct MINMAX
    {
    float min;
    float max;
    };

struct ANALOG_SENSOR_SETUP_DATA
    {
    float     minval;
    float     maxval;
    float     mul;
    float     sub;
    float     div;
    float     add;
    float     m_volt_con;
    float     coef[MAX_POLY_COEF];
    short     units;
    BITSETYPE vartype;
    short     gain;
    };

struct POS_SENSOR_SETUP_DATA
    {
    float x4_pos_divisor;    /* Position units per unit measure */
    float x1_pos_divisor;    /* Marks per unit measure */
    float vel_dividend;      /* Divide by count to obtain velocity */
    float min_vel;           /* Min vel in units for rod pitch */
    float crystal_freq;      /* FasTrak crystal freq. in MHz. */
    };

struct CURVE_DATA
    {
    BITSETYPE type;
    float     max;
    float     min;
    short     units;
    short     ft_channel_number();
    BITSETYPE ft_channel_type();
    };

#define _PROFILE_H_

struct PROFILE_HEADER
    {
    TCHAR      machine_name[MACHINE_NAME_LEN+1];
    TCHAR      part_name[PART_NAME_LEN+1];
    int32      shot_number;
    FILETIME   time_of_shot;      /* 8 bytes */
    short      n;
    short      last_pos_based_point; /* NofPosBasedPoints - 1 */
    CURVE_DATA curves[MAX_CURVES];
    };

struct MACHINE_NAME_ENTRY
    {
    TCHAR   computer[COMPUTER_NAME_LEN+1];
    TCHAR   machine[MACHINE_NAME_LEN+1];

    MACHINE_NAME_ENTRY();
    void    empty( void );
    BOOLEAN is_empty( void );
    BOOLEAN is_equal( const TCHAR * cn, const TCHAR * mn );
    BOOLEAN operator==(const MACHINE_NAME_ENTRY & sorc ) { return is_equal(sorc.computer, sorc.machine); }
    void    operator=(const MACHINE_NAME_ENTRY & sorc );
    void    set( const TCHAR * cn, const TCHAR * mn );
    void    set( const TCHAR * mn );
    };

struct PART_NAME_ENTRY
    {
    TCHAR computer[COMPUTER_NAME_LEN+1];
    TCHAR machine[MACHINE_NAME_LEN+1];
    TCHAR part[PART_NAME_LEN+1];

    BOOLEAN exists( void );
    BOOLEAN is_equal( const TCHAR * cn, const TCHAR * mn, const TCHAR * pn );
    BOOLEAN is_machine( const TCHAR * mn );
    BOOLEAN is_part( const TCHAR * pn );
    void set( const TCHAR * cn, const TCHAR * mn, const TCHAR * pn );
    void set( const TCHAR * pn );
    PART_NAME_ENTRY();
    BOOLEAN operator==(const PART_NAME_ENTRY & sorc );
    void    operator=(const PART_NAME_ENTRY & sorc );
    };

struct PROFILE_NAME_ENTRY
    {
    TCHAR computer[COMPUTER_NAME_LEN+1];
    TCHAR machine[MACHINE_NAME_LEN+1];
    TCHAR part[PART_NAME_LEN+1];
    TCHAR shot[SHOT_LEN+1];
    void  set( const TCHAR * cn, const TCHAR * mn, const TCHAR * pn, const TCHAR * sn );
    void  operator=(const PROFILE_NAME_ENTRY & sorc );
    void  operator=(const PART_NAME_ENTRY & sorc );
    };

struct SHOT_ENTRY
    {
    SYSTEMTIME time;
    int32      number;
    };

struct SHOT_RANGE_ENTRY
    {
    SHOT_ENTRY newest;
    SHOT_ENTRY oldest;
    };

#endif

