struct WORKER_ENTRY
    {
    WORKER_ENTRY * next;
    TCHAR number[OPERATOR_NUMBER_LEN+1];
    TCHAR firstname[OPERATOR_FIRST_NAME_LEN+1];
    TCHAR lastname[OPERATOR_LAST_NAME_LEN+1];
    };

class WORKER_LIST_CLASS
    {
    public:

    WORKER_ENTRY * list;
    WORKER_ENTRY * current_entry;
    int32          n;

    void    cleanup( void );
    WORKER_LIST_CLASS() { n = 0; list = 0; current_entry = 0; }
    ~WORKER_LIST_CLASS() { cleanup(); }

    BOOLEAN add( TCHAR * new_number, TCHAR * new_firstname, TCHAR * new_lastname );
    BOOLEAN find( TCHAR * number_to_find );
    BOOLEAN load( void );
    TCHAR * full_name( TCHAR * number_to_find );
    TCHAR * first_name();
    TCHAR * last_name();
    };
