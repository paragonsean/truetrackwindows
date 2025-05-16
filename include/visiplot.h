#ifndef _VISI_PLOT_SOURCE_

extern TCHAR VisiPlotClassName[];
extern TCHAR VisiXAxisClassName[];
extern TCHAR VisiYAxisClassName[];

void register_visi_plot( HINSTANCE  main_instance );
LRESULT CALLBACK visi_plot_proc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#endif

const UINT WM_LOW_VISIPLOT_MSG_VALUE  = WM_USER+0x200;
const UINT WM_ADD_VISIPLOT_DATA       = WM_USER+0x200; /* wParam low = curve, high = n, lParam = * POINT (I delete) */
const UINT WM_SET_VISIPLOT_PEN_WIDTH  = WM_USER+0x201; /* wParam = curve, lParm = width */
const UINT WM_SET_VISIPLOT_PEN_COLOR  = WM_USER+0x202; /* wParam = curve, lParam = color */
const UINT WM_SET_VISIPLOT_PEN_STYLE  = WM_USER+0x203; /* wParam = curve, lParam = style */
const UINT WM_SET_VISIPLOT_LIMITS     = WM_USER+0x204; /* wParam = curve, lParam = * RECT (I delete)*/
const UINT WM_CLEAR_VISIPLOT_CURVE    = WM_USER+0x205; /* wParam = curve */
const UINT WM_HIGH_VISIPLOT_MSG_VALUE = WM_USER+0x205;

const UINT WM_LOW_AXIS_MSG_VALUE      = WM_USER+0x220;
const UINT WM_SET_AXIS_PLOT_WINDOW    = WM_USER+0x220; /* wParam = plot window handle */
const UINT WM_SET_AXIS_PEN_COLOR      = WM_USER+0x221; /* wParam = new pen color      */
const UINT WM_SET_NOF_MAJOR_TICS      = WM_USER+0x222; /* wParam = number of tics     */
const UINT WM_SET_AXIS_TYPE           = WM_USER+0x223; /* wParam = axis type          */
const UINT WM_SET_NOF_MINOR_TICS      = WM_USER+0x224; /* wParam = number of tics     */
const UINT WM_SET_REAL_AXIS_LIMITS    = WM_USER+0x225; /* wParam = point to double[2] = min, max (I delete) */
const UINT WM_HIGH_AXIS_MSG_VALUE     = WM_USER+0x225;

const BITSETYPE X_AXIS_TYPE     = 1;
const BITSETYPE LEFT_AXIS_TYPE  = 2;
const BITSETYPE RIGHT_AXIS_TYPE = 4;

class REAL_POINT
    {
    public:
    double x;
    double y;

    REAL_POINT() { x = 0.0; y = 0.0; }
    REAL_POINT( double a, double b ) { x = a; y = b; }
    REAL_POINT( const REAL_POINT & sorc ) { x = sorc.x; y = sorc.y; }
    const REAL_POINT & operator=( const REAL_POINT & sorc ) { x = sorc.x; y = sorc.y; return *this; }
    ~REAL_POINT() {}
    };

class REAL_RECT
    {
    public:
    double left;
    double top;
    double right;
    double bottom;
    void set( double a, double b, double c, double d ) { left = a; top = b; right = c; bottom = d; }
    REAL_RECT() { left = 0.0; top = 0.0; right = 0.0; bottom = 0.0; }
    REAL_RECT( double a, double b, double c, double d ) { left = a; top = b; right = c; bottom = d; }
    REAL_RECT( const REAL_RECT & sorc ) { left = sorc.left; top = sorc.top; right = sorc.right; bottom = sorc.bottom;}
    const REAL_RECT & operator=( const REAL_RECT & sorc ) { left = sorc.left; top = sorc.top; right = sorc.right; bottom = sorc.bottom; return *this; }
    ~REAL_RECT() {}
    };

class PEN_ENTRY
    {
    public:
    COLORREF color;
    int      style;
    int      width;
    PEN_ENTRY() { color = RGB( 0, 0, 0); style = PS_SOLID; width=1; }
    void operator=( const PEN_ENTRY & sorc );
    };

class CURVE_ENTRY
    {
    public:
    PEN_ENTRY pen;
    REAL_POINT * pa;
    int       n;
    int       max_n;
    REAL_RECT extent;
    double    delta_x;
    double    delta_y;

    CURVE_ENTRY();
    ~CURVE_ENTRY();
    void transfer( CURVE_ENTRY & sorc );
    void set_extents( double x1, double y1, double x2, double y2 );
    void set_extents( REAL_RECT & sorc );
    bool add( HWND w, int number_of_new_points, REAL_POINT * p );
    void clear( void );
    bool draw( HDC dc, HWND w, RECT & r, int first_index, int last_index );
    bool draw( HDC dc, HWND w, RECT & r ) { return draw(dc, w, r, 0, n-1 ); }
    };

class VISI_PLOT_ENTRY
    {
    public:
    int           nof_curves;
    CURVE_ENTRY * curve;
    VISI_PLOT_ENTRY()  { nof_curves = 0; curve = 0; }
    ~VISI_PLOT_ENTRY();
    bool curve_exists( int curve_index );
    };

class VISI_AXIS_ENTRY
    {
    public:

    BITSETYPE type;
    PEN_ENTRY pen;
    HWND      plot_window_handle;
    long      minimum; /* pos of left or bottom edge of plot window */
    long      maximum; /* pos of right or top edge of plot window */
    double    real_minimum;
    double    real_maximum;
    int       left_places;
    int       right_places;
    int       nof_major_tics;
    int       nof_minor_tics;
    VISI_AXIS_ENTRY();
    void draw_left_axis( HDC dc, HWND w, RECT & r );
    void draw_right_axis( HDC dc, HWND w, RECT & r );
    void draw_x_axis( HDC dc, HWND w, RECT & r );
    bool draw( HDC dc, HWND w );
    void set_plot_window( HWND my_handle, HWND new_plot_window_handle );
    };

class VISIAXIS_WINDOW_CLASS : public WINDOW_CLASS
    {
    public:

    VISIAXIS_WINDOW_CLASS();
    ~VISIAXIS_WINDOW_CLASS();
    void set_plot_window( HWND w );
    void set_pen_color( COLORREF pencolor );
    void set_tic_count( int nof_major_tics, int nof_minor_tics );
    void set_axis_limits( double minimum, double maximum );
    void set_axis_type( BITSETYPE new_type );
    void operator=( const HWND sorc ) { WINDOW_CLASS::operator=(sorc); }
    };

class VISIPLOT_WINDOW_CLASS : public WINDOW_CLASS
    {
    private:

    public:

    VISIPLOT_WINDOW_CLASS();
    ~VISIPLOT_WINDOW_CLASS();
    void add( int curve_number, int nof_points, REAL_POINT * p );
    void clear_curve( int curve_number );
    void set_pen_color( int curve_number, COLORREF pencolor );
    void set_pen_width( int curve_number, int width );
    void set_plot_limits( int curve_number, double x_min, double x_max, double y_min, double y_max );
    void operator=( const HWND sorc ) { WINDOW_CLASS::operator=(sorc); }
    };

