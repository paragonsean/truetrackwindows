#ifndef _FTII_FILE_
#define _FTII_FILE_

STRING_CLASS & replace_value( TCHAR * sorc, TCHAR * new_value );
STRING_CLASS & replace_value( TCHAR * sorc, int new_value );
STRING_CLASS & replace_value( TCHAR * sorc, unsigned new_value );
STRING_CLASS & replace_percent_value( TCHAR * sorc, double new_value, BOOLEAN need_bit_30 );
STRING_CLASS & replace_percent_value( TCHAR * sorc, float new_value, BOOLEAN need_bit_30 );
STRING_CLASS & replace_percent_value( TCHAR * sorc, double new_value );
STRING_CLASS & replace_percent_value( TCHAR * sorc, float new_value );
int      signed_variable_value( TCHAR * sorc );
int      variable_number( const TCHAR * sorc );
unsigned variable_value( const TCHAR * sorc );

#endif

