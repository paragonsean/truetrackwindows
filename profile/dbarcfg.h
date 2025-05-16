const BITSETYPE DTR_ALL_RANGE       = 1;
const BITSETYPE DTR_LAST_N_RANGE    = 2;
const BITSETYPE DTR_START_DATE_RANGE = 4;
const BITSETYPE DTR_END_DATE_RANGE   = 8;

class DOWNBAR_CONFIG_CLASS
{
public:
BITSETYPE  range_type;
int32      nof_days;
BOOLEAN    show_all_subs;
BOOLEAN    sort_subs_by_cat;
TIME_CLASS start;
TIME_CLASS end;

DOWNBAR_CONFIG_CLASS();
~DOWNBAR_CONFIG_CLASS();
void read( void );
void write( void );
void    operator=( const DOWNBAR_CONFIG_CLASS & sorc );
BOOLEAN operator==(const DOWNBAR_CONFIG_CLASS & sorc );
BOOLEAN operator!=(const DOWNBAR_CONFIG_CLASS & sorc );
};

