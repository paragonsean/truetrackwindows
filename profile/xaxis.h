#ifndef _X_AXIS_CLASS_
#define _X_AXIS_CLASS_

#ifndef _ARRAY_CLASS_
#include "..\include\array.h"
#endif

#ifndef _BITMAPCL_H_
#include "bitmapcl.h"
#endif


#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif
 
#ifndef _TIME_CLASS_
#include "..\include\timeclas.h"
#endif
 
const BOOLEAN WITH_BITMAP = TRUE;
const BOOLEAN NO_BITMAP   = FALSE;

const int32 NO_AXIS_TYPE        = 0;
const int32 INTEGER_AXIS_TYPE   = 1;

class SHOT_DATA_ENTRY
    {
    public:

    int32      shot_number;
    TIME_CLASS shot_time;
 
    SHOT_DATA_ENTRY() { shot_number = NO_SHOT_NUMBER; }
    void set( int32 new_shot_number, SYSTEMTIME & st ) { shot_number = new_shot_number; shot_time.set(st); }
    };

class SHOT_ARRAY_CLASS
    {
    private:
    static TIME_CLASS notime;
 
    protected:

    int32 x;
    int32 n;
    int32 nset;
    BOOLEAN rewound;
    SHOT_DATA_ENTRY * p;

    public:

    void    cleanup( void );
    SHOT_ARRAY_CLASS() { p = 0; cleanup(); }
    ~SHOT_ARRAY_CLASS() { cleanup(); }
    int32 count( void ) { return n; }
    int32   index( void ) { return x; }
    virtual BOOLEAN init( int32 nof_points );
    int32   left_shift( SHOT_DATA_ENTRY & sorc );
    int32   left_shift( int32 shotno, SYSTEMTIME & st );
    int32   max_shot_number( void );
    int32   min_shot_number( void );
    BOOLEAN is_set( void ) { if ( x < nset ) return TRUE; return FALSE; }
    BOOLEAN next( void );
    BOOLEAN next_set( void );
    void    rewind( void ) { x = 0; rewound = TRUE; }
    BOOLEAN set( int32 shotno, SYSTEMTIME & st );
    int32   set_count( void ) { return nset; }
    SHOT_DATA_ENTRY & operator[]( int32 i );

    int32        shot_number( void ) { if ( p ) return p[x].shot_number;  else return 0;   }
    TIME_CLASS & shot_time( void )   { if ( p ) return p[x].shot_time;    else return notime; }
    };

class X_AXIS_CLASS : public SHOT_ARRAY_CLASS
    {
    private:
    HDC     dc;

    int32 intercept;
    int32 type;
    int32 nof_tics;
    COLORREF axis_color;
    COLORREF label_color;
    COLORREF title_color;

    RECT r;
    STRING_CLASS title;

    int32   min_pixel;
    int32   max_pixel;
    int32   pixel_range;

    BOOLEAN firstime;
    int32   max;
    int32   min;
    int32   data_range;

    void draw_line( int x1, int y1, int x2, int y2 );
    int32 info_box_x_position( int32 x );

    public:

    BITMAP_CLASS bitmap;

    X_AXIS_CLASS();
    ~X_AXIS_CLASS();

    BOOLEAN init( int32 nof_ints );
    void left_shift( int32 new_shot_number, SYSTEMTIME & st );
    BOOLEAN restore_info_box( HDC dc );
    void    set( int32 new_shot_number, SYSTEMTIME & st );
    void    set_axis_color(  COLORREF c ) { axis_color = c;  }
    void    set_label_color( COLORREF c ) { label_color = c; }
    void    set_rect( RECT & new_rect )   { r = new_rect;    }
    void    set_title_color( COLORREF c ) { title_color = c; }
    void    set_type( int32 new_type )    { type        = new_type; }
    void    set_nof_tics( int32 n )       { nof_tics    = n; }
    void    set_title( TCHAR * s )        { title = s; }
    int32   x_index( int32 x_pixel_pos );
    int32   x_pos( int32 x );
    int32   x_pos_from_index( int32 x );
    int32   y_intercept( void ) { return intercept; }
    BOOLEAN paint( HDC dest_device_context );
    BOOLEAN show_info_box( HDC dc, int32 x, int32 i, BOOLEAN need_bitmap );
    };

#endif
