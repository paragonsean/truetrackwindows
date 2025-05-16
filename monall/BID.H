
struct BOARD_INITIALIZATION_DATA
    {
    short nof_time_samples;
    float total_stroke_len;
    float min_stroke_len;
    float biscuit_time_delay;
    float eos_velocity;
    float sleeve_fill_distance;
    float runner_fill_distance;
    float min_csfs_vel;
    float rise_csfs_vel;
    float min_csfs_pos;
    float limits[LIMIT_SWITCH_COUNT];
    float ms_per_time_based_sample;
    float auto_shot_timeout_seconds;
    };

BOOLEAN get_board_initialization_data( BOARD_INITIALIZATION_DATA & dest, TCHAR * computer, TCHAR * machine, TCHAR * part );


