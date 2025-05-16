#ifndef _FONT_CLASS_
#define _FONT_CLASS_

const int NO_FONT_ID = -1;
const HFONT INVALID_FONT_HANDLE = (HFONT) INVALID_HANDLE_VALUE;

#ifdef UNICODE
class FONT_CLASS : public tagLOGFONTW
#else
class FONT_CLASS : public tagLOGFONTA
#endif
    {
    public:
    FONT_CLASS();
    FONT_CLASS( const FONT_CLASS & f );
    FONT_CLASS( const LOGFONT & f );
    ~FONT_CLASS();
    void operator=(const FONT_CLASS & sorc );
    void operator=(const LOGFONT & sorc );
    bool operator==(const FONT_CLASS & sorc );
    bool operator==(const LOGFONT & sorc );
    bool    get( TCHAR *sorc );
    TCHAR * put();
    bool    choose( HWND w );
    bool    choose( HWND w, COLORREF & color );
    };

struct FONT_LIST_ENTRY
    {
    FONT_LIST_ENTRY * next;
    int               reference_count;
    HFONT             handle;
    FONT_CLASS        font;
    FONT_LIST_ENTRY();
    ~FONT_LIST_ENTRY();
    };

class FONT_LIST_CLASS
    {
    private:
    FONT_LIST_ENTRY * first;

    public:
    FONT_LIST_CLASS();
    ~FONT_LIST_CLASS();
    bool  get_font( FONT_CLASS & dest, HFONT handle_to_get );
    HFONT get_handle( const FONT_CLASS & font_desired );
    void  free_handle( HFONT handle_to_remove );
    };

#endif

