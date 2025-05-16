struct MACHINE_ENTRY
    {
    TCHAR              name[MACHINE_NAME_LEN+1];
    TCHAR              computer[COMPUTER_NAME_LEN+1];
    GENERIC_LIST_CLASS partlist;
    };

struct PART_ENTRY
    {
    TCHAR name[PART_NAME_LEN+1];
    };

void            cleanup_machines_and_parts( void );
MACHINE_ENTRY * find_machine_entry( TCHAR * machine );
PART_ENTRY    * find_part_entry( MACHINE_ENTRY * m, TCHAR * part );
int             machine_count( TCHAR * computer_to_search );
BOOLEAN         machine_is_local( TCHAR * ma );
BOOLEAN         read_machines_and_parts( void );
BOOLEAN         reload_partlist( TCHAR * machine );

