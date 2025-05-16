#ifndef _FIFO_CLASS_
#define _FIFO_CLASS_

struct FIFO_ENTRY
    {
    FIFO_ENTRY * next;
    FIFO_ENTRY * prev;
    void       * item;
    };

class FIFO_CLASS
    {
    private:

    CRITICAL_SECTION   my_critical_section;
    FIFO_ENTRY * first;
    FIFO_ENTRY * last;
    int          n;

    public:
    FIFO_CLASS();
    ~FIFO_CLASS();
    BOOLEAN   push( void * new_item );
    int       count() { return n; }
    void    * peek();
    void    * pop();
    };

#endif

