class FIXED_PARAM_CLASS
{
public:
TCHAR  max_value[MAX_DOUBLE_LEN+1];
TCHAR  min_value[MAX_DOUBLE_LEN+1];
bool   is_fixed;

FIXED_PARAM_CLASS();
FIXED_PARAM_CLASS( const FIXED_PARAM_CLASS & sorc );
~FIXED_PARAM_CLASS();
FIXED_PARAM_CLASS & operator=( const FIXED_PARAM_CLASS & sorc );
void    get_from_line( TCHAR * sorc );
TCHAR * lbline( TCHAR * parameter_name );
void    read(  TCHAR * parameter_name );
void    write( TCHAR * parameter_name );
void    write_from_line( TCHAR * sorc );
};
