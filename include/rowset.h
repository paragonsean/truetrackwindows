#ifndef _ROW_SET_CLASS_
#define _ROW_SET_CLASS_

struct ROW_SET_ENTRY
{
ROW_SET_ENTRY * next;
int32 low;
int32 high;
};

class ROW_SET_CLASS
{
private:
ROW_SET_ENTRY * RowBase;
ROW_SET_ENTRY * ce;
int32           high_value;
int32           low_value;

BOOLEAN         append( ROW_SET_ENTRY * r );

public:

ROW_SET_CLASS() { RowBase = 0; ce = 0; low_value = 0; high_value = 0; }
~ROW_SET_CLASS();

BOOLEAN add( int32 new_low, int32 new_high );
BOOLEAN contains( int32 row );
int32   high( void ) { return high_value; }
BOOLEAN insert( ROW_SET_ENTRY * r );
int32   low( void )  { return low_value;  }
BOOLEAN next( void );
void    remove( void );
void    rewind( void ) { ce = 0; }

};

#endif
