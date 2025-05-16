const int32 TREND_CHART_TYPE        = 0;
const int32 XBAR_CHART_TYPE         = 1;
const int32 RANGE_CHART_TYPE        = 2;
const int32 HISTOGRAM_CHART_TYPE    = 3;

const int32 SHOT_NUMBER_X_AXIS_TYPE = 0;
const int32 HOUR_DAY_X_AXIS_TYPE    = 1;
const int32 SHIFT_DAY_X_AXIS_TYPE   = 2;
const int32 DAY_MONTH_X_AXIS_TYPE   = 3;

const int32 SHOT_AVERAGE_TYPE       = 0;
const int32 MINUTE_AVERAGE_TYPE     = 1;
const int32 HOUR_AVERAGE_TYPE       = 2;
const int32 SHIFT_AVERAGE_TYPE      = 3;
const int32 DAY_AVERAGE_TYPE        = 4;
const int32 WEEK_AVERAGE_TYPE       = 5;
const int32 MONTH_AVERAGE_TYPE      = 6;

const int32 LAST_N_TIME_PERIOD_TYPE     = 0;
const int32 SHOT_RANGE_TIME_PERIOD_TYPE = 1;

class TREND_CONFIG_CLASS
{
public:

int32     chart_type;
int32     nto_average;
int32     average_over_type;
int32     time_period_type;
int32     nof_points;
int32     first_shot;
int32     last_shot;
int32     x_axis_type;
BOOLEAN   need_alarm_limits;
BOOLEAN   need_warning_limits;
BOOLEAN   need_ucl;
BOOLEAN   need_lcl;
BOOLEAN   visible[MAX_PARMS];
float     ucl_sigma_value;
float     lcl_sigma_value;

TREND_CONFIG_CLASS();
~TREND_CONFIG_CLASS();
void read( void );
void write( void );
int32   n_visible();
void    operator=( const TREND_CONFIG_CLASS & sorc );
BOOLEAN operator==(const TREND_CONFIG_CLASS & sorc );
BOOLEAN operator!=(const TREND_CONFIG_CLASS & sorc );
};