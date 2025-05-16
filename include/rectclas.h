#ifndef _RECTANGLE_CLASS_
#define _RECTANGLE_CLASS_

#ifndef BITSETYPE
typedef unsigned short BITSETYPE;
#endif

const BITSETYPE N_BORDER  = 1;
const BITSETYPE E_BORDER  = 2;
const BITSETYPE S_BORDER  = 4;
const BITSETYPE W_BORDER  = 8;

class RECTANGLE_CLASS : public tagRECT
    {
    public:
    RECTANGLE_CLASS();
    RECTANGLE_CLASS( const RECTANGLE_CLASS & r );
    RECTANGLE_CLASS( const RECT & r );
    RECTANGLE_CLASS( long x1, long y1, long x2, long y2 );
    ~RECTANGLE_CLASS();
    void      operator=(const RECTANGLE_CLASS & sorc );
    void      operator=(const RECT & sorc );
    BOOLEAN   operator==(const RECTANGLE_CLASS & sorc );
    BOOLEAN   operator==(const RECT & sorc );
    BOOLEAN   contains( const POINT & p );
    int       getcursor(const POINT & p );
    void      empty( void ) { right = left; bottom = top; }
    BOOLEAN   get( TCHAR * sorc );
    BOOLEAN   intersects( const RECT & r );
    BOOLEAN   is_empty( void );
    BITSETYPE near_borders( const POINT & p );
    void      moveto( POINT & p );
    void      moveto( long x,  long y );
    void      offset( long dx, long dy );
    void      offset( POINT & p ) { offset( p.x, p.y ); }
    TCHAR *   put();
    void      setsize( SIZE sz );
    void      setsize( long width, long height );
    void      set( long x1, long y1, long x2, long y2 ) { left=x1; top=y1; right=x2; bottom=y2; }
    long      height( void ) { return bottom - top;  }
    long      width( void )  { return right  - left; }
    void      draw( HDC dc );
    void      draw( HDC dc, COLORREF color );
    void      fill( HDC dc, HBRUSH   brush );
    void      fill( HDC dc, COLORREF color );
    };

#endif

