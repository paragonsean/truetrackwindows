#ifndef _RUNLIST_CLASS_
#define _RUNLIST_CLASS_

struct RUNLIST_ENTRY
    {
    unsigned long bits;
    TCHAR         partname[PART_NAME_LEN+1];
    RUNLIST_ENTRY() { bits = 0; *partname = TEXT( '\0' ); }
    };

class RUNLIST_CLASS
    {
    private:

    RUNLIST_ENTRY * list;
    int             n;
    unsigned int    mask;

    void cleanup();

    public:

    RUNLIST_CLASS() { list = 0; n = 0; mask = 0; }
    RUNLIST_CLASS( const RUNLIST_CLASS & sorc );
    ~RUNLIST_CLASS();
    int count() { return n; }
    unsigned bitmask() { return mask; }
    RUNLIST_ENTRY & operator[]( int i );
    TCHAR * current_part_name();
    TCHAR * find_part( unsigned int suretrak_bits );
    BOOLEAN get( TCHAR * tablename );
    BOOLEAN get( TCHAR * co, TCHAR * ma );
    };

#endif
