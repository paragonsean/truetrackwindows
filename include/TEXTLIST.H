#ifndef _TEXT_LIST_CLASS_
#define _TEXT_LIST_CLASS_

#ifndef _GENLIST_H
#include "..\include\genlist.h"
#endif

class TEXT_LIST_CLASS
    {
    private:
    TCHAR            * current_entry;
    GENERIC_LIST_CLASS tlist;

    public:
    TEXT_LIST_CLASS( void );
    ~TEXT_LIST_CLASS( void );
    void       operator=( TEXT_LIST_CLASS & sorc );
    BOOLEAN    append( TCHAR * sorc, int32 slen );
    BOOLEAN    append( TCHAR * sorc ) { return append(sorc, lstrlen(sorc) ); }
    int32      count( void ) { return (int32) tlist.count(); }
    void       empty( void );
    BOOLEAN    find_exact( TCHAR * s );
    BOOLEAN    find( TCHAR * s, int32 slen );
    BOOLEAN    find( TCHAR * s ) { return find(s, (int32) lstrlen(s)); }
    void       get_directory_list( const TCHAR * rootdir );
    void       get_file_list( const TCHAR * rootdir, const TCHAR * file_name_pattern );
    void       get_file_list( const TCHAR * pattern_path );
    TCHAR    * release();
    BOOLEAN    remove( void );
    void       rewind( void ) { tlist.rewind(); current_entry = 0; }
    BOOLEAN    next( void );
    bool       split( const TCHAR * sorc, const TCHAR * separator );
    BOOLEAN    sort( void );
    TCHAR    * text( void ) { return current_entry; }
    };

#endif
