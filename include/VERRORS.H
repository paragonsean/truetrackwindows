/*
-----------
Subroutines
----------- */
#define BOARD_STARTUP                   1
#define UNITS_STARTUP                   2
#define MACHINE_CLASS_LOADPART          3
#define MACHINE_CLASS_SET_ANALOG_SENSOR 4
#define GET_POLY_EQ_NUMBER              5
#define LOAD_PARMDATA                   6
#define FIND_ANALOG_SENSOR              7
#define MSG_FT_SETPORTS                 8
#define MSG_SET_DELAY                   9
#define MSG_GET_VERSION                 10
#define MSG_SET_CRYSTAL_FREQ            11
#define MSG_SET_TIME_OUT_PERIOD         12
#define MSG_SET_TIM_COLL_INTERVAL       13
#define MSG_FT_SET_TYPE                 14
#define MSG_SET_TOT_STK_LEN             15
#define MSG_SET_MIN_STK_LEN             16
#define MSG_SET_TIM_DEL_BISC            17
#define MSG_SET_VEL_EOS                 18
#define MSG_SET_FILL_DISTANCE           19
#define MSG_SET_RUN_FILL_DISTANCE       20
#define MSG_SET_MIN_VEL_CSFS            21
#define MSG_SET_VEL_RISE_TIME           22
#define MSG_SET_POS_CSFS                23
#define MSG_SET_LIM1_SETPT              24
#define MSG_SET_LIM2_SETPT              25
#define MSG_SET_LIM3_SETPT              26
#define MSG_SET_LIM4_SETPT              27
#define MSG_SET_MIN_VEL_EOS             28
#define FTINIT_ROUTINE                  29
#define DATA_COLLECTION_MODE            30
#define FIND_POSITION_SENSOR            31
#define LOAD_SENSOR_POINTERS            32
#define GET_BOARD_INITIALIZATION_DATA   33
#define ANALOG_SENSOR_STARTUP           34
#define READ_COUNTS                     35
#define FIND_CURRENT_MACHINE            36
#define READ_PARAMETER_SUB              37
#define FILL_MACHINES_SUB               38
#define FILL_DCURVES_SUB                39
#define COLOR_RELOAD_SUB                40
#define SKIP_FIND                       41
#define SKIP_RESET                      42

/*
------
Errors
------ */
#define NO_MEM_ALLOC_ERROR              1
#define MACHINE_NOT_FOUND               2
#define PART_NOT_FOUND                  3
#define SENSOR_NOT_FOUND                4
#define FT_BOARD_ERROR                  5
#define FT_BOARD_NOT_FOUND              6
#define PARM_LOAD_ERROR                 7
#define DB_ERROR                        8
#define NOT_FOUND                       9
#define NO_DATA_FOUND                  10
#define CREATE_FAILURE                 11
#define NO_CURRENT_PART                12

