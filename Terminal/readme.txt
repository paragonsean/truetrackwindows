FasTrak II Monitor Setpoints

| V318 | TOT_STK_LEN_SET         |   x4 count
| V319 | MIN_STK_LEN_SET         |   x4 count
| V320 | SET_MIN_VEL_EOS         |   x1 counts per second
| V321 | TIM_DEL_BISC_SET        |   number of time based intervals
| V322 | VEL_EOS_SET             |   x1 counts per second
| V323 | SET_FILL_DIST           |   x4 count
| V324 | SET_RUN_FILL_DIST       |   x4 count
| V325 | SET_MIN_POS_CSFS        |   x4 count
| V326 | SET_VEL_CSFS            |   x1 counts per second
| V327 | SET_VEL_RISE_TIME       |   x1 counts per second
| V328 | SET_TIM_COLL_INTERVAL   |   ms
| V329 | SET_DELAY               |   number of post impact points
| V330 | SET_TIME_OUT_PERIOD     |   ms

| V366 | SET_LS1                 |   x4 count
| V367 | SET_LS2                 |   x4 count
| V368 | SET_LS3                 |   x4 count
| V369 | SET_LS4                 |   x4 count
| V370 | SET_LS5                 |   x4 count
| V371 | SET_LS6                 |   x4 count

1 => 0-5.0 Volts
0 => 0-2.5 Volts

| V313 | Bit 0 | 0000 0001 | CH1 VOLTS |
| V313 | Bit 1 | 0000 0010 | CH2 VOLTS |
| V313 | Bit 2 | 0000 0100 | CH3 VOLTS |
| V313 | Bit 3 | 0000 1000 | CH4 VOLTS |
| V313 | Bit 4 | 0001 0000 | CH5 VOLTS |
| V313 | Bit 5 | 0010 0000 | CH6 VOLTS |
| V313 | Bit 6 | 0100 0000 | CH7 VOLTS |
| V313 | Bit 7 | 1000 0000 | CH8 VOLTS |

struct NEW_FTII_DATA {
    SOCKET sorc;
    int    type;
    void * buf;
    int    len;
    };

set_time_based_points( NEW_FTII_DATA nd );
set_position_based_points( n, p );
set_parameters( n, p );

File format for FTII data

short nof_recs
  1         short curve type, int min, int max
  2         short curve type, int min, int max
  ...
  nof_recs  short curve type, int min, int max

short nof_curves
short nof_pos_based_points       ( n = nof_pos_based_points + nof_time_based+points )
short nof_time_based_points
short nof_calculated_parameters

int curve type
    data for this curve

int curve type
    data for this curve

etc.

curve types
  1  Position
  2  Time (position based only )
  3  Time (all points)
  4  Q1   (position based only)
  5  Q2            "
  6  Q3            "
  7  Q4            "
  8  ISW1
  9  ISW2
 10  ISW3
 11  ISW4
 12  OSW1
 13  OSW2
 14  OSW3
101  A1
102  A2
...

curve type     2 bytes
unsigned short 2 bytes
int            4 bytes
unsigned int   4 bytes

             Point Size      Number of Points        Description
A1, A2, ...  unsigned short  n                       Analog Inputs
TP           unsigned int    nof_pos_based_points    Time in MS, Position Based Only
TB           unsigned int    n                       Time in MS, All Points
Q1, ..., Q4  unsigned int    nof_pos_based_points    Velocity Counts
IW1, ...     unsigned int    n                       Digital Inputs
OW1, ...     unsigned int    n                       Digital Outputs
P            int             n                       Position

