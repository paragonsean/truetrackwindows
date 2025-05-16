#ifndef _PARAM_INDEX_CLASS_
#define _PARAM_INDEX_CLASS_

class PARAM_INDEX_CLASS
    {
    private:
    int   x[MAX_PARMS];

    public:
    PARAM_INDEX_CLASS();
    ~PARAM_INDEX_CLASS();
    void init();
    int  & operator[]( int i );
    int  parameter_number( int parameter_index );
    int  index( int parameter_number );
    bool move( int new_index, int current_index );
    bool load( TCHAR * co, TCHAR * ma, TCHAR * pa );
    bool load( PART_NAME_ENTRY & pn );
    bool save( TCHAR * co, TCHAR * ma, TCHAR * pa );
    bool save( PART_NAME_ENTRY & pn );
    };

#endif