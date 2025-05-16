#ifndef _COLOR_CLASS_
#define _COLOR_CLASS_

class COLOR_CLASS
    {
    public:
    COLORREF color;

    COLOR_CLASS() { color = 0; }
    COLOR_CLASS( const COLOR_CLASS & sorc ) { color = sorc.color; }
    COLOR_CLASS( int r, int g, int b ) { set( r, g, b ); }
    ~COLOR_CLASS() {}
    int red_component();
    int green_component();
    int blue_component();
    void operator=( COLORREF sorc ) { color = sorc; }
    void operator=( COLOR_CLASS sorc ) { color = sorc.color; }
    TCHAR * put();
    void get( TCHAR * sorc );
    void set( int red, int green, int blue ) { color = RGB( (BYTE) red, (BYTE) green, (BYTE) blue ); }
    };

#endif
