#ifndef _PLOT_CLASS_
#define _PLOT_CLASS_

const BITSETYPE GRID_TIC_TYPE = 1;
const BITSETYPE LINE_TIC_TYPE = 2;
const BITSETYPE NO_TIC_TYPE   = 0;

const BITSETYPE HORIZONTAL_AXIS_TYPE = 1;
const BITSETYPE VERTICAL_AXIS_TYPE   = 2;

const int32 X_AXIS_INDEX  = 0;
const int32 Y_AXIS_INDEX  = 1;
const int32 NOF_BASE_AXES = 2;

struct BASE_AXIS_ENTRY
    {
    int32              start_pos;
    int32              end_pos;
    int32              nof_tics;
    int32              tic_length;
    COLORREF           color;
    GENERIC_LIST_CLASS labels;
    };

struct LABEL_ENTRY
    {
    POINT         pos;
    COLORREF      color;
    STRING_CLASS  t;
    };

struct AXIS_ENTRY
    {
    int32              id;
    int32              base_axis_index;
    COLORREF           color;
    float              max_value;
    float              min_value;
    int32              decimal_places;
    };

struct FLOAT_POINT
    {
    float x;
    float y;
    };

struct CURVE_ENTRY
    {
    int32 xid;
    int32 yid;
    int32 n;
    FLOAT_POINT * data;
    };

class PLOT_CLASS
    {
    private:

    HWND w;
    HDC  dc;
    RECT               perimeter;
    BOOLEAN            need_to_show_symbols;
    BOOLEAN            have_new_label;
    BOOLEAN            have_new_axis;
    BOOLEAN            have_new_curve;
    BASE_AXIS_ENTRY    base[NOF_BASE_AXES];
    GENERIC_LIST_CLASS axes;
    GENERIC_LIST_CLASS curves;

    void    draw_x_tics( void );
    void    draw_y_tics( void );
    BOOLEAN value_to_pixel_equation( float & a, float & b, int32 id );
    int32   widest_char( TCHAR * sorc );

    public:

    PLOT_CLASS();
    ~PLOT_CLASS();
    BOOLEAN      add_label( int32 base_axis, TCHAR * sorc, COLORREF new_color );
    BOOLEAN      add_axis(  int32 id, int32 base_axis, float min_value, float max_value, COLORREF new_color );
    BOOLEAN      add_curve(  int32 xid, int32 yid, int32 nof_points, FLOAT_POINT * fp );
    void         cleanup( void );
    void         draw_axes( void );
    void         draw_curves( void );
    void         empty_axes( void );
    void         empty_curves( void );
    void         empty_labels( void );
    AXIS_ENTRY * findaxis( int32 id_to_find );
    BOOLEAN      get_pixel_equation( float & a, float & b, int32 axis_id );
    void         paint( HDC dc );
    void         refresh( void );
    void         set_tic_count( int32 axis, int32 new_count );
    void         set_decimal_places( int32 id_to_set, int32 new_count );
    void         show_labels( void );
    void         dont_show_symbols( void ) { need_to_show_symbols = FALSE; }
    void         size( HWND hWnd );
    void         size( HWND hWnd, RECT bounds );
    };

#endif
