#ifndef _BITMAPCL_H_
#define _BITMAPCL_H_
 
class BITMAP_CLASS
    {
    private:
 
    HBITMAP bh;
 
    int32   h;
    int32   w;
    int32   x;
    int32   y;
 
    public:
    BITMAP_CLASS();
    ~BITMAP_CLASS();
    void    cleanup( void );
    int32   height( void ) { return h; }
    BOOLEAN init( HDC dc, int32 new_width, int32 new_height );
    BOOLEAN get( HDC dc, int32 xpos, int32 ypos );
    BOOLEAN restore( HDC dc );
    int32   width( void ) { return w; }
    };
 
#endif
 
