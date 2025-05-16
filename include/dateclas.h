#ifndef _DATE_CLASS_
#define _DATE_CLASS_

class DATE_CLASS
    {
    private:

    SYSTEMTIME st;

    TCHAR      s[ALPHADATE_LEN+1];

    public:

    DATE_CLASS();
    WORD         year( void )  { return st.wYear;  }
    WORD         month( void ) { return st.wMonth; }
    WORD         day( void )   { return st.wDay;   }
    WORD         dow( void )   { return st.wDayOfWeek; }
    TCHAR      * text( void )  { return s; }
    void operator=( const DATE_CLASS & sorc );
    void operator=( const SYSTEMTIME & sorc );
    BOOLEAN      operator==( const DATE_CLASS & sorc );

    SYSTEMTIME * system_time( void );
    };

#endif
