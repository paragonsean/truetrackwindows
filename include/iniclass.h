#ifndef _INI_CLASS_
#define _INI_CLASS_

class INI_CLASS
    {
    private:

    TCHAR  * filename;
    TCHAR  * section_name;
    TCHAR  * buf;
    BOOLEAN  my_file_exists;

    public:

    INI_CLASS() { section_name = 0; filename=0; buf=0; my_file_exists=FALSE; }
    ~INI_CLASS();
    void    cleanup();
    BOOLEAN set_file( const TCHAR * new_filename );
    BOOLEAN set_section( const TCHAR * new_section );
    BOOLEAN exists() { return my_file_exists; }
    BOOLEAN find( const TCHAR * section, const TCHAR * key );
    BOOLEAN find( const TCHAR * key );

    BOOLEAN get_boolean();
    BOOLEAN get_boolean( const TCHAR * key );
    BOOLEAN get_boolean( const TCHAR * section, const TCHAR * key );

    int     get_int();
    int     get_int( const TCHAR * key );
    int     get_int( const TCHAR * section, const TCHAR * key );

    double  get_double();
    double  get_double( const TCHAR * key );
    double  get_double( const TCHAR * section, const TCHAR * key );

    TCHAR * get_string();
    TCHAR * get_string( const TCHAR * key );
    TCHAR * get_string( const TCHAR * section, const TCHAR * key );

    unsigned long get_ul();
    unsigned long get_ul( const TCHAR * key );
    unsigned long get_ul( const TCHAR * section, const TCHAR * key );

    BOOLEAN put_string( const TCHAR * dest_file_name, const TCHAR * section, const TCHAR * key, const TCHAR * sorc );
    BOOLEAN put_string( const TCHAR * section, const TCHAR * key, const TCHAR * sorc );
    BOOLEAN put_string( const TCHAR * key, const TCHAR * sorc );
    BOOLEAN put_boolean(const TCHAR * dest_file_name, const TCHAR * section, const TCHAR * key, BOOLEAN sorc );
    BOOLEAN put_boolean(const TCHAR * section, const TCHAR * key, BOOLEAN sorc );
    BOOLEAN put_boolean(const TCHAR * key, BOOLEAN sorc );

    BOOLEAN put_int( const TCHAR * key, int sorc );
    BOOLEAN put_int( const TCHAR * section, const TCHAR * key, int sorc );

    BOOLEAN put_ul( const TCHAR * key, unsigned long sorc );
    BOOLEAN put_ul( const TCHAR * section, const TCHAR * key, unsigned long sorc );

    BOOLEAN put_double( const TCHAR * key, double sorc );
    BOOLEAN put_double( const TCHAR * section, const TCHAR * key, double sorc );
    void    remove_key( const TCHAR * key );
    void    remove_key( const TCHAR * section, const TCHAR * key );
    };

#endif
