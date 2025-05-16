#ifndef _NEW_MACHINE_STATE_CLASS_
#define _NEW_MACHINE_STATE_CLASS_

class NEW_MACHINE_STATE_CLASS
    {
    private:

    TCHAR name[MACHINE_NAME_LEN+1];
    BOARD_DATA old_status_value;
    BOARD_DATA new_status_value;

    public:

    NEW_MACHINE_STATE_CLASS() { lstrcpy( name, NO_MACHINE ); old_status_value = new_status_value = 0; }
    ~NEW_MACHINE_STATE_CLASS() {}
    TCHAR    * machine_name( void )  { return name; }
    BOARD_DATA new_status( void ) { return new_status_value; }
    BOARD_DATA old_status( void ) { return old_status_value; }
    BOOLEAN    set( HDDEDATA edata );
    };

#endif

#ifndef _DOWN_DATA_CLASS_
#define _DOWN_DATA_CLASS_

class DOWN_DATA_CLASS
    {
    private:

    TCHAR machine[MACHINE_NAME_LEN+1];
    TCHAR part[PART_NAME_LEN+1];
    SYSTEMTIME st;
    TCHAR cat[DOWNCAT_NUMBER_LEN+1];
    TCHAR subcat[DOWNCAT_NUMBER_LEN+1];
    TCHAR catname[DOWNCAT_NAME_LEN+1];
    TCHAR subcatname[DOWNCAT_NAME_LEN+1];
    TCHAR opnumber[OPERATOR_NUMBER_LEN+1];

    public:

    DOWN_DATA_CLASS();
    ~DOWN_DATA_CLASS() {}
    void  operator=(const DOWN_DATA_CLASS & sorc );
    TCHAR    * machine_name( void )  { return machine; }
    TCHAR    * part_name( void )  { return part; }
    TCHAR    * category( void )  { return cat; }
    TCHAR    * subcategory( void )  { return subcat; }
    TCHAR    * category_name( void )  { return catname; }
    TCHAR    * subcategory_name( void )  { return subcatname; }
    TCHAR    * operator_number( void )  { return opnumber; }
    SYSTEMTIME * system_time() { return &st; }
    BOOLEAN    set( HDDEDATA edata );
    };

#endif

