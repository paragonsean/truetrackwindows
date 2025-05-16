#ifndef _PASSWORD_CLASS_

#define _PASSWORD_CLASS_

struct PASSWORD_ENTRY
    {
    int32 level;
    TCHAR name[PASSWORD_LEN+1];
    TCHAR owner[PASSWORD_OWNER_LEN+1];
    PASSWORD_ENTRY() { level=0; *name=TEXT('\0'); *owner=TEXT('\0'); }
    ~PASSWORD_ENTRY(){}
    };

class PASSWORD_CLASS
    {
    private:

    int32            n;

    public:

    PASSWORD_ENTRY * pw;

    PASSWORD_CLASS() { pw=0; n=0; }
    ~PASSWORD_CLASS();
    int32   count() { return n; }
    PASSWORD_ENTRY & operator[]( int32 i );
    void    cleanup();
    int32   default_level();
    int32   highest_level();
    int32   find( TCHAR * password_to_find );
    bool    setsize( int32 new_count );
    bool    load();
    bool    save();
    };

#endif
