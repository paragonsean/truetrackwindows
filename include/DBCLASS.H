#ifndef _DB_CLASS_
#define _DB_CLASS_

#define SEARCHFIRST     1
#define SEARCHNEXT      2

#define VS_SUCCESS      0

#define WITH_LOCK       TRUE
#define NO_LOCK         FALSE

#define NO_RETRIES      0

#define ALPHA_ADDED_CHARS 3        /* Field = alpha len +""; */
#define NUMBER_ADDED_CHARS 1        /* Field = number len +; */

#define DATE_LEN       10         /* 12/25/1996 */

/*
-----------------
Get_alpha options
----------------- */
#define WITH_TRIM       1

/*
----------------
Table Lock Types
---------------- */
#define PFL  1
#define PWL  2
#define WL   4
#define FL   8

typedef unsigned short FIELDOFFSET;
typedef unsigned short BITSETYPE;

class DB_TABLE
    {

    private:

    TCHAR   * name;
    HANDLE    th;
    TCHAR   * recbuf;
    TCHAR   * searchbuf;
    short     recbuflen;          /* Buffer Length NOT counting NULL */
    short     searchlen;
    int32     recnum;
    BITSETYPE table_lock_type;
    BOOLEAN   have_table_lock;
    BOOLEAN   have_rec_lock;
    BITSETYPE search_mode;
    DWORD     dberr;
    DWORD     globalerr;
    BOOLEAN   compare_one_record( short * same, int32 rn, int32 nof_key_fields );
    DWORD     current_record_offset() { return recnum * sizeof(TCHAR) * DWORD(recbuflen); }
    BOOLEAN   fill_searchbuf( short nof_key_fields );
    BOOLEAN   goto_key_record( short nof_key_fields, int32 firstrec, int32 lastrec );
    BOOLEAN   goto_first_key_record( short nof_key_fields );
    BOOLEAN   initbuf( TCHAR ** buf, short * buflen, short slen );
    LPTSTR    point_to_start_of_field( FIELDOFFSET fo );
    void      restore_from_searchbuf();
    BOOLEAN   write_record();

    public:
    DB_TABLE();
    ~DB_TABLE();
    int32   current_record_number() { return recnum; }
    void    close();
    BOOLEAN copy_rec( DB_TABLE & sorcdb );
    BOOLEAN create( TCHAR * dbname );
    BOOLEAN empty();
    BOOLEAN ensure_existance( TCHAR * dbname );
    BOOLEAN get_current_record( BOOLEAN need_lock );
    DWORD   get_global_error() { return globalerr; }
    DWORD   get_last_error()   { return dberr; }
    BOOLEAN get_alpha( LPTSTR dest, FIELDOFFSET fo, short max_len, BITSETYPE options );
    BOOLEAN get_alpha( LPTSTR dest, FIELDOFFSET fo, short max_len );
    BOOLEAN get_boolean( FIELDOFFSET fo );
    TCHAR   get_char( FIELDOFFSET fo );
    BOOLEAN get_date( SYSTEMTIME & dest, FIELDOFFSET fo );
    double  get_double( FIELDOFFSET fo );
    float   get_float(  FIELDOFFSET fo )
        {
        return (float) get_double( fo );
        }
    long    get_long(   FIELDOFFSET fo );
    BOOLEAN get_text_field( TCHAR * dest, FIELDOFFSET fo );
    BOOLEAN get_next_key_match( short nof_key_fields, BOOLEAN need_lock );
    BOOLEAN get_next_record( BOOLEAN need_lock );
    BOOLEAN get_prev_record( BOOLEAN need_lock );
    BOOLEAN get_rec();
    short   get_short( FIELDOFFSET fo )
        {
        return (short) get_long( fo );
        }
    BOOLEAN get_time( SYSTEMTIME & dest, FIELDOFFSET fo );
    BOOLEAN goto_first_greater_than_record( short nof_key_fields );
    BOOLEAN goto_first_equal_or_greater_record( short nof_key_fields );
    BOOLEAN goto_record_number( int32 n );
    BOOLEAN lock_record();
    DWORD   nof_recs();
    BOOLEAN open( TCHAR * dbname, short reclen, short locktype, short time_out );
    BOOLEAN open( TCHAR * dbname, short reclen, short locktype );
    BOOLEAN open_status();
    BOOLEAN put_alpha( FIELDOFFSET fo, const TCHAR * sorc, short field_len );
    BOOLEAN put_boolean( FIELDOFFSET fo, BOOLEAN sorc );
    BOOLEAN put_char( FIELDOFFSET fo, TCHAR sorc );
    BOOLEAN put_date( FIELDOFFSET fo, const SYSTEMTIME & sorc );
    BOOLEAN put_double( FIELDOFFSET fo, const double sorc, short field_len );
    BOOLEAN put_float(  FIELDOFFSET fo, const float sorc,  short field_len );
    BOOLEAN put_long( FIELDOFFSET fo, const long sorc, short field_len );
    BOOLEAN put_short( FIELDOFFSET fo, const short sorc, short field_len )
        {
        return put_long( fo, (long) sorc, field_len );
        }
    BOOLEAN put_time( FIELDOFFSET fo, SYSTEMTIME & sorc );
    BOOLEAN rec_append();
    BOOLEAN rec_delete( int32 ntokill );
    BOOLEAN rec_delete() { return rec_delete( 1 ); }
    BOOLEAN rec_insert();
    BOOLEAN rec_update();
    short   record_length() { return recbuflen; }
    void    reset_search_mode() { search_mode = SEARCHFIRST; }
    void    rewind() { search_mode = SEARCHFIRST; }
    BOOLEAN unlock_record();
    };
#endif
