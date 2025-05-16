#ifndef _TIME_CLASS_
#define _TIME_CLASS_

const unsigned long SECONDS_PER_YEAR   = 31536000UL;
const unsigned long SECONDS_PER_DAY    = 86400UL;
const unsigned long SECONDS_PER_HOUR   = 3600UL;
const unsigned long SECONDS_PER_MINUTE = 60UL;

class TIME_CLASS
    {
    private:

    unsigned long n;

    public:

    TIME_CLASS() { n=0; }
    BOOLEAN set( const SYSTEMTIME & t );
    BOOLEAN set( const FILETIME & f );
    BOOLEAN set( unsigned long new_seconds ) { n = new_seconds; return TRUE; }
    void    set_time( const TCHAR * sorc );
    FILETIME & file_time();
    SYSTEMTIME & system_time();
    void get_local_time();
    void operator=( const TIME_CLASS & sorc ) { n = sorc.n; }
    void operator=( const TCHAR * sorc );
    void operator+=( const TIME_CLASS & sorc) { n += sorc.n; }
    void operator+=(unsigned long sorc) { n += sorc; }
    void operator-=(unsigned long sorc);
    BOOLEAN operator>( const TIME_CLASS & sorc ) { return (n>sorc.n); }
    BOOLEAN operator>=( const TIME_CLASS & sorc ) { return (n>=sorc.n); }
    BOOLEAN operator<( const TIME_CLASS & sorc ) { return (n<sorc.n); }
    BOOLEAN operator<=( const TIME_CLASS & sorc ) { return (n<=sorc.n); }
    BOOLEAN operator==(const TIME_CLASS & sorc ) { return (n==sorc.n); }
    BOOLEAN operator!=(const TIME_CLASS & sorc ) { return (n!=sorc.n); }
    long    operator-(const TIME_CLASS & sorc );
    unsigned long current_value() { return n; }
    TCHAR * text();
    TCHAR * mmddyyyy();
    TCHAR * mmddyy();
    TCHAR * hhmmss();
    TCHAR * dd();
    TCHAR * mm();
    TCHAR * mmm();
    TCHAR * yy();
    TCHAR * yyyy();
    TCHAR * hhmm();
    TCHAR * hh();
    TCHAR * mi();
    };

#endif
