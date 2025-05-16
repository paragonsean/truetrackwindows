#define GET_FIRST_COMPUTER 0
#define GET_NEXT_COMPUTER  1

#define COMPUTER_NAME_LEN 3
#define NO_COMPUTER       TEXT( "NOC" )

struct COMPUTER_ENTRY
{
COMPUTER_ENTRY * next;
TCHAR   name[COMPUTER_NAME_LEN+1];
TCHAR   directory[MAX_PATH+1];
BOOLEAN online;
};

class COMPUTER_CLASS
{
private:

COMPUTER_ENTRY * c;
short            cx;
short            search_mode;

public:

COMPUTER_CLASS() { c=0; cx = 0; search_mode = GET_FIRST_COMPUTER; }
BOOLEAN add( TCHAR * new_name, TCHAR * new_directory );
short   count();
TCHAR * whoami();
TCHAR * directory( TCHAR * name );
TCHAR * directory();
TCHAR * name();
void    empty();
BOOLEAN find( TCHAR * name );
BOOLEAN find_directory( const TCHAR * dir_to_find );
BOOLEAN is_present();
BOOLEAN is_present( TCHAR * co );
BOOLEAN is_this_computer();
BOOLEAN load();
void    rewind();
BOOLEAN next();
BOOLEAN online_status();
BOOLEAN online_status( TCHAR * co );
BOOLEAN save();
BOOLEAN set_online_status( BOOLEAN new_status );
BOOLEAN set_online_status( TCHAR * co, BOOLEAN new_status );
BOOLEAN startup();
BOOLEAN startup( TCHAR * exepath );
void    shutdown();
};
