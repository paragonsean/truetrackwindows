#ifndef _VTWCONTROL_CLASS_
#define _VTWCONTROL_CLASS_

#ifndef _FILE_CLASS_
#include "..\include\fileclas.h"
#endif

#ifndef _RECTANGLE_CLASS_
#include "..\include\rectclas.h"
#endif

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

#ifndef _WINDOW_CLASS_
#include "..\include\wclass.h"
#endif

const unsigned int BORDER_RECTANGLE_TYPE = 0x01;
const unsigned int FILLED_RECTANGLE_TYPE = 0x02;
const unsigned int TEXT_TYPE             = 0x04;
const unsigned int BAR_TYPE              = 0x40;
const unsigned int CONTROL_TYPE_MASK     = 0x47;

const unsigned int CENTER_VERTICALLY     = 0x08;
const unsigned int CENTER_HORIZONTALLY   = 0x10;
const unsigned int RIGHT_JUSTIFY         = 0x80;
const unsigned int ALIGNMENT_MASK        = 0x98;

const unsigned int BACKGROUND_TYPE       = 0x20;

/*
-------------------------------
Flag values for CONTROL_ENTRY's
------------------------------- */
const unsigned int FIXED_LIMITS_FLAG     = 0x01;

/*
---------------------
WM_NEW_DATA set types
--------------------- */
const WPARAM SET_VALUE     = 0x00;
const WPARAM SET_MAX_VALUE = 0x01;
const WPARAM SET_MIN_VALUE = 0x02;

/*
--------------------
MS_CONTROL constants
-------------------- */
const DWORD BUTTON_MASK = 0x1F;

class MS_CONTROL_ENTRY
    {
    public:
    DWORD           style;
    bool            is_static;
    bool            is_groupbox;
    int             id;
    HFONT           font_handle;
    STRING_CLASS    name;
    STRING_CLASS    s;
    RECTANGLE_CLASS r;
    WNDPROC         winproc;

    MS_CONTROL_ENTRY() { style = 0; id = -1; font_handle = INVALID_FONT_HANDLE; winproc = 0; is_groupbox=false; is_static=false; }
    ~MS_CONTROL_ENTRY() {}
    void operator=( const MS_CONTROL_ENTRY & sorc );
    bool is_radio_button();
    bool is_editbox();
    bool is_defpushbutton();
    bool read( FILE_CLASS & f );
    bool write( FILE_CLASS & f );
    HWND create( HWND parent, HINSTANCE main_instance );
    HWND create( WINDOW_CLASS & parent, HINSTANCE main_instance ) { return create( parent.handle(), main_instance ); }
    };

class CONTROL_ENTRY
    {
    public:
    unsigned int    type;
    COLORREF        color;
    COLORREF        font_color;
    HFONT           font_handle;
    int             id;
    int             current_value;
    int             min_value;
    int             max_value;
    unsigned int    flags;
    STRING_CLASS    s;
    RECTANGLE_CLASS r;
    CONTROL_ENTRY() { color = RGB( 255, 0, 0 ); type = BORDER_RECTANGLE_TYPE; id = -1; font_color=0; font_handle = INVALID_FONT_HANDLE; flags=0; min_value=0; max_value=100; current_value=0; }
    ~CONTROL_ENTRY() {}
    void operator=( const CONTROL_ENTRY & sorc );
    bool read( FILE_CLASS & f );
    bool write( FILE_CLASS & f );
    HWND create( HWND parent, HINSTANCE main_instance );
    HWND create( WINDOW_CLASS & parent, HINSTANCE main_instance ) { return create( parent.handle(), main_instance ); }
    };

#endif
