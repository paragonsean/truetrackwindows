#define _VISI_MARKS_H_

#define MAX_MARKS (MAX_PARMS + USER_AVG_VEL_COUNT + USER_AVG_VEL_COUNT + USER_VEL_COUNT + FT_ONOFF_COUNT + FT_ONOFF_COUNT)

/*
-------
Symbols
------- */
#define VLINE_MARK  1
#define BLINE_MARK  2
#define NOLINE_MARK 4

/*
---------------
Parameter types
--------------- */
#define PARAMETER_MARK TEXT( 'P' )
#define START_POS_MARK TEXT( 'S' )
#define END_POS_MARK   TEXT( 'E' )
#define VEL_POS_MARK   TEXT( 'V' )

struct MARKLIST_ENTRY
    {
    TCHAR       parameter_type;
    short       parameter_number;
    short       color;
    BITSETYPE   symbol;
    TCHAR       label[MARK_LABEL_LEN+1];
    };

class MARKS_CLASS
    {
    private:
    short          new_index( TCHAR type_to_add, short number_to_add );
    void           set_defaults( short i );

    public:

    TCHAR          computer[COMPUTER_NAME_LEN+1];
    TCHAR          machine[MACHINE_NAME_LEN+1];
    TCHAR          part[PART_NAME_LEN+1];
    short          n;
    MARKLIST_ENTRY array[MAX_MARKS];

    MARKS_CLASS( void );
    void           add( TCHAR type_to_add, short number_to_add );
    short          index( TCHAR type_to_find, short number_to_find );
    BOOLEAN        load( TCHAR * computer, TCHAR * machine, TCHAR * part );
    void           remove( short i );
    BOOLEAN        save( void );
    };

