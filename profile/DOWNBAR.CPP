#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\dbclass.h"
#include "..\include\listbox.h"
#include "..\include\names.h"
#include "..\include\stringcl.h"
#include "..\include\textlen.h"
#include "..\include\textlist.h"
#include "..\include\timeclas.h"
#include "..\include\subs.h"

#include "extern.h"
#include "resource.h"
#include "dbarcfg.h"
#include "downclas.h"

extern STRING_CLASS ArialFontName;
extern TCHAR   DefaultArialFontName[];

class CAT_BASE_CLASS
    {
    public:
    TCHAR cat[DOWNCAT_NUMBER_LEN+1];

    CAT_BASE_CLASS();
    CAT_BASE_CLASS( TCHAR * new_cat );
    void    operator=( const CAT_BASE_CLASS & sorc );
    void    operator=( const TCHAR * sorc ) { lstrcpy( cat, sorc ); }
    BOOLEAN operator==(const CAT_BASE_CLASS & sorc );
    BOOLEAN operator==(const TCHAR * sorc );
    BOOLEAN operator!=(const CAT_BASE_CLASS & sorc );
    };

class CAT_SUM_ENTRY : public CAT_BASE_CLASS
    {
    public:
    CAT_SUM_ENTRY * next;
    int32 sum;

    CAT_SUM_ENTRY() { next = 0; sum = 0; }
    };

class CAT_SUM_CLASS
    {
    private:
    CAT_SUM_ENTRY * p;

    int32   n;
    CAT_SUM_ENTRY * base;

    public:
    void    cleanup( void );
    int32   count( void ) { return n; }
    CAT_SUM_ENTRY * find( TCHAR * cat );
    void    rewind( void ) { p = 0; }
    BOOLEAN next( void );
    int32 & seconds( TCHAR * cat );
    int32   seconds( void );
    void    sort( void );
    TCHAR * title( void );
    TCHAR * category( void );

    CAT_SUM_CLASS() { p = 0; base = 0; n = 0; }
    CAT_SUM_ENTRY * add( TCHAR * new_cat );
    };

class DOWNTIME_SUM_ENTRY : public DOWNTIME_BASE_CLASS
    {
    public:
    int32 sum;

    DOWNTIME_SUM_ENTRY();
    };

class DOWNTIME_SUM_CLASS
    {
    private:

    int32   x;
    TCHAR   searchcat[DOWNCAT_NUMBER_LEN+1];
    int32   searchsum;

    int32   n;
    DOWNTIME_SUM_ENTRY * p;

    public:

    TIME_CLASS start;
    TIME_CLASS end;

    void cleanup( void );
    DOWNTIME_SUM_CLASS() { p=0; n=0; x = 0; lstrcpy(searchcat, NO_DOWNCAT_NUMBER); searchsum = 0; }
    ~DOWNTIME_SUM_CLASS() { cleanup(); }
    int32   count( void ) { return n; }
    BOOLEAN read( void );
    BOOLEAN empty( void );
    BOOLEAN next( TCHAR * cat );
    BOOLEAN next( void ) { return next(0); }
    void    rewind( void ) { x = 0; searchsum = 0; }
    BOOLEAN operator==( const DOWNTIME_BASE_CLASS & sorc );
    BOOLEAN operator!=( const DOWNTIME_BASE_CLASS & sorc );
    int32 & seconds( TCHAR * cat, TCHAR * subcat );
    int32 total_seconds( void );
    int32 down_seconds( void );
    int32 seconds( void );
    TCHAR * title( void );
    TCHAR * category( void );
    };

DOWNBAR_CONFIG_CLASS Config;

static const double REAL_SECONDS_PER_HOUR = 3600.0;

TEXT_LIST_CLASS Titles;

int LeftX;
int HoursX;
int PercentX;
double TotalHours = 1.0;

long TopButtonY;
long ButtonHeight;

DOWNTIME_BASE_CLASS NoCode( NO_DOWNCAT_NUMBER, NO_DOWNCAT_NUMBER );
DOWNTIME_BASE_CLASS UpCode( DOWNCAT_SYSTEM_CAT, DOWNCAT_UP_SUBCAT );
CAT_BASE_CLASS NoCategory( NO_DOWNCAT_NUMBER );
CAT_BASE_CLASS CurrentCategory( NO_DOWNCAT_NUMBER );

extern HDC  MyDc;
extern RECT PlotWindowRect;
extern BOOLEAN Printing;

static TCHAR NullChar      = TEXT( '\0' );
static TCHAR SpaceString[] = TEXT( " " );

DOWNTIME_SUM_CLASS Dh;
CAT_SUM_CLASS      Cs;

TCHAR * resource_string( UINT resource_id );

/***********************************************************************
*                            CAT_BASE_CLASS                            *
***********************************************************************/
CAT_BASE_CLASS::CAT_BASE_CLASS( TCHAR * new_cat )
{
lstrcpy( cat, new_cat );
}

/***********************************************************************
*                            CAT_BASE_CLASS                            *
***********************************************************************/
CAT_BASE_CLASS::CAT_BASE_CLASS()
{
lstrcpy( cat, NO_DOWNCAT_NUMBER );
}

/***********************************************************************
*                            CAT_BASE_CLASS                            *
*                                 =                                    *
***********************************************************************/
void CAT_BASE_CLASS::operator=( const CAT_BASE_CLASS & sorc )
{
lstrcpy( cat, sorc.cat );
}

/***********************************************************************
*                            CAT_BASE_CLASS                            *
*                                  ==                                  *
***********************************************************************/
BOOLEAN CAT_BASE_CLASS::operator==( const TCHAR * sorc )
{
if ( lstrcmp(cat, sorc) == 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            CAT_BASE_CLASS                            *
*                                  ==                                  *
***********************************************************************/
BOOLEAN CAT_BASE_CLASS::operator==( const CAT_BASE_CLASS & sorc )
{
return operator==( sorc.cat );
}

/***********************************************************************
*                            CAT_BASE_CLASS                            *
*                                !=                                    *
***********************************************************************/
BOOLEAN CAT_BASE_CLASS::operator!=( const CAT_BASE_CLASS & sorc )
{
if ( lstrcmp(cat, sorc.cat) == 0 )
    return FALSE;

return TRUE;
}

/***********************************************************************
*                            CAT_SUM_CLASS                             *
*                              cleanup                                 *
***********************************************************************/
void CAT_SUM_CLASS::cleanup( void )
{
while ( base )
    {
    p    = base;
    base = base->next;
    delete p;
    }

n = 0;
p = 0;
}

/***********************************************************************
*                            CAT_SUM_CLASS                             *
*                                next                                  *
***********************************************************************/
BOOLEAN CAT_SUM_CLASS::next( void )
{
if ( p )
    p = p->next;
else
    p = base;

if ( p )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                            CAT_SUM_CLASS                             *
*                               seconds                                *
***********************************************************************/
int32 CAT_SUM_CLASS::seconds( void )
{
if ( p )
    return p->sum;

return 0;
}

/***********************************************************************
*                            CAT_SUM_CLASS                             *
*                                title                                 *
***********************************************************************/
TCHAR * CAT_SUM_CLASS::title( void )
{
static TCHAR emptystring[] = TEXT("");

if ( p )
    return category_name( p->cat );

return emptystring;
}

/***********************************************************************
*                            CAT_SUM_CLASS                             *
*                                add                                   *
***********************************************************************/
CAT_SUM_ENTRY * CAT_SUM_CLASS::add( TCHAR * new_cat )
{
CAT_SUM_ENTRY * np;

np = new CAT_SUM_ENTRY;
if ( np )
    {
    lstrcpy( np->cat, new_cat );
    np->sum = 0;
    n++;

    if ( base )
        {
        p = base;
        while ( p->next )
            p = p->next;
        p->next = np;
        }
    else
        {
        base = np;
        }
    }

return np;
}

/***********************************************************************
*                            CAT_SUM_CLASS                             *
*                              category                                *
***********************************************************************/
TCHAR * CAT_SUM_CLASS::category( void )
{
static TCHAR nocat[] = NO_DOWNCAT_NUMBER;

if ( p )
    return p->cat;

return nocat;
}

/***********************************************************************
*                            CAT_SUM_CLASS                             *
*                                find                                  *
***********************************************************************/
CAT_SUM_ENTRY * CAT_SUM_CLASS::find( TCHAR * cat )
{
CAT_SUM_ENTRY * cp;

cp = base;
while ( cp )
    {
    if ( *cp == cat )
        return cp;
    cp = cp->next;
    }

return 0;
}

/***********************************************************************
*                            CAT_SUM_CLASS                             *
*                               SECONDS                                *
***********************************************************************/
int32 & CAT_SUM_CLASS::seconds( TCHAR * cat )
{
static int32 dummy = 0;
CAT_SUM_ENTRY * cp;

cp = find( cat );
if ( !cp )
    cp = add( cat );

if ( cp )
    return cp->sum;

return dummy;
}

/***********************************************************************
*                            CAT_SUM_CLASS                             *
*                                SORT                                  *
***********************************************************************/
void CAT_SUM_CLASS::sort( void )
{
CAT_SUM_ENTRY * newbase;
CAT_SUM_ENTRY * prev;
CAT_SUM_ENTRY * copy;

newbase = 0;

while ( base )
    {
    copy = base;
    base = base->next;
    copy->next = 0;

    if ( !newbase )
        {
        newbase = copy;
        }
    else
        {
        prev = 0;
        p    = newbase;
        while ( TRUE )
            {
            if ( p->sum > copy->sum )
                {
                copy->next = p;
                if ( prev )
                    prev->next = copy;
                else
                    newbase = copy;
                break;
                }
            else if ( !p->next )
                {
                p->next = copy;
                break;
                }
            else
                {
                prev = p;
                p = p->next;
                }
            }
        }
    }

base = newbase;
}

/***********************************************************************
*                         DOWNTIME_BASE_CLASS                          *
***********************************************************************/
DOWNTIME_BASE_CLASS::DOWNTIME_BASE_CLASS()
{
lstrcpy( cat,    NO_DOWNCAT_NUMBER );
lstrcpy( subcat, NO_DOWNCAT_NUMBER );
}

/***********************************************************************
*                         DOWNTIME_BASE_CLASS                          *
***********************************************************************/
DOWNTIME_BASE_CLASS::DOWNTIME_BASE_CLASS( TCHAR * new_cat, TCHAR * new_subcat )
{
lstrcpy( cat,    new_cat    );
lstrcpy( subcat, new_subcat );
color = 0;
}

/***********************************************************************
*                         DOWNTIME_BASE_CLASS                         *
*                                 =                                    *
***********************************************************************/
void DOWNTIME_BASE_CLASS::operator=( const DOWNTIME_BASE_CLASS & sorc )
{
lstrcpy( cat,    sorc.cat );
lstrcpy( subcat, sorc.subcat );
color = sorc.color;
}

/***********************************************************************
*                         DOWNTIME_BASE_CLASS                          *
*                                  !=                                  *
***********************************************************************/
BOOLEAN DOWNTIME_BASE_CLASS::operator!=( const DOWNTIME_BASE_CLASS & sorc )
{
if ( lstrcmp(cat, sorc.cat) != 0 || lstrcmp(subcat, sorc.subcat) != 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                         DOWNTIME_BASE_CLASS                          *
*                                   ==                                 *
***********************************************************************/
BOOLEAN DOWNTIME_BASE_CLASS::operator==( const DOWNTIME_BASE_CLASS & sorc )
{
if ( lstrcmp(cat, sorc.cat) == 0 && lstrcmp(subcat, sorc.subcat) == 0 )
    return TRUE;

return FALSE;
}

/***********************************************************************
*                        DOWNTIME_RECORD_CLASS                         *
*                                GET                                   *
***********************************************************************/
BOOLEAN DOWNTIME_RECORD_CLASS::get( DB_TABLE & t )
{
SYSTEMTIME st;

init_systemtime_struct( st );

t.get_date( st, DOWNTIME_DATE_OFFSET );
t.get_time( st, DOWNTIME_TIME_OFFSET );

tm.set( st );

t.get_alpha( cat,    DOWNTIME_CAT_OFFSET,       DOWNCAT_NUMBER_LEN );
t.get_alpha( subcat, DOWNTIME_SUBCAT_OFFSET,    DOWNCAT_NUMBER_LEN );

return TRUE;
}

/***********************************************************************
*                        DOWNTIME_RECORD_CLASS                         *
*                                 =                                    *
***********************************************************************/
void DOWNTIME_RECORD_CLASS::operator=( const DOWNTIME_RECORD_CLASS & sorc )
{
DOWNTIME_BASE_CLASS::operator=( sorc );
tm = sorc.tm;
}

/***********************************************************************
*                        DOWNTIME_SUM_ENTRY                            *
***********************************************************************/
DOWNTIME_SUM_ENTRY::DOWNTIME_SUM_ENTRY()
{
sum = 0;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                              CLEANUP                                 *
***********************************************************************/
void DOWNTIME_SUM_CLASS::cleanup( void )
{
if ( p )
    {
    delete[] p;
    p = 0;
    }

n = 0;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                               READ                                   *
***********************************************************************/
BOOLEAN DOWNTIME_SUM_CLASS::read( void )
{
DB_TABLE  t;
int32     nrecs;
BOOLEAN   status;
TCHAR   * filename;
short     record_length;
TCHAR     buf[DOWNSCATC_COLOR_LEN+1];

cleanup();
status = FALSE;

filename = downscatc_dbname();
if ( file_exists(filename) )
    {
    record_length = DOWNSCATC_RECLEN;
    }
else
    {
    record_length = DOWNSCAT_RECLEN;
    filename = downscat_dbname();
    }

if ( file_exists(filename) )
    {
    if ( t.open(filename, record_length, PFL) )
        {
        nrecs = t.nof_recs();
        if ( nrecs > 0 )
            {
            n = 0;
            p = new DOWNTIME_SUM_ENTRY[nrecs];
            if ( p )
                {
                status = TRUE;
                while( t.get_next_record(NO_LOCK) )
                    {
                    t.get_alpha( p[n].cat,    DOWNSCAT_CAT_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );
                    t.get_alpha( p[n].subcat, DOWNSCAT_SUB_NUMBER_OFFSET, DOWNCAT_NUMBER_LEN );

                    if ( record_length == DOWNSCATC_RECLEN )
                        {
                        t.get_alpha( buf, DOWNSCATC_COLOR_OFFSET, DOWNSCATC_COLOR_LEN   );
                        p[n].color = (COLORREF) hextoul( buf );
                        }
                    n++;
                    if ( n >= nrecs )
                        break;
                    }
                }
            }
        t.close();
        }
    }

x = 0;
searchsum = 0;
return status;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                              EMPTY                                   *
***********************************************************************/
BOOLEAN DOWNTIME_SUM_CLASS::empty( void )
{
int32 i;

if ( p )
    {
    for ( i=0; i<n; i++ )
        p[i].sum = 0;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                                 !=                                   *
***********************************************************************/
BOOLEAN DOWNTIME_SUM_CLASS::operator!=( const DOWNTIME_BASE_CLASS & sorc )
{
if ( p && x >= 0 && x < n )
    if ( p[x] == sorc )
        return FALSE;

return TRUE;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                                 ==                                   *
***********************************************************************/
BOOLEAN DOWNTIME_SUM_CLASS::operator==( const DOWNTIME_BASE_CLASS & sorc )
{
if ( p && x >= 0 && x < n )
    if ( p[x] == sorc )
        return TRUE;

return FALSE;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                              SECONDS                                 *
***********************************************************************/
int32 & DOWNTIME_SUM_CLASS::seconds( TCHAR * cat, TCHAR * subcat )
{
static int32 no_seconds;
int32 i;

if ( p )
    {
    for ( i=0; i<n; i++ )
        {
        if ( lstrcmp(p[i].cat, cat) == 0 && lstrcmp(p[i].subcat, subcat) == 0 )
            return p[i].sum;
        }
    }

return no_seconds;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                              SECONDS                                 *
***********************************************************************/
int32 DOWNTIME_SUM_CLASS::seconds( void )
{
if ( p )
    return p[x].sum;

return 0;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                              CATEGORY                                *
***********************************************************************/
TCHAR * DOWNTIME_SUM_CLASS::category( void )
{
static TCHAR s[DOWNCAT_NUMBER_LEN+1] = NO_DOWNCAT_NUMBER;

if ( p )
    return p[x].cat;

return s;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                               TITLE                                  *
***********************************************************************/
TCHAR * DOWNTIME_SUM_CLASS::title( void )
{
static TCHAR s[DOWNCAT_NAME_LEN+1+DOWNCAT_NAME_LEN+1];

s[0] = NullChar;

if ( p )
    {
    lstrcpy( s, subcategory_name(p[x].cat, p[x].subcat) );
    }

return s;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                                NEXT                                  *
***********************************************************************/
BOOLEAN DOWNTIME_SUM_CLASS::next( TCHAR * cat )
{
int32 i;

int32 nexti;
int32 nextsum;


if ( !p )
    return FALSE;

nexti   = x;
nextsum = searchsum;

for ( i=0; i<n; i++ )
    {
    if ( i == x )
        continue;

    if ( p[i].sum == 0 )
        continue;

    if ( cat )
        if ( lstrcmp(p[i].cat, cat) != 0 )
            continue;

    if ( p[i].sum < searchsum )
        continue;

    if ( p[i].sum > searchsum )
        {
        if ( nexti != x && p[i].sum >= nextsum )
            continue;
        nexti = i;
        nextsum = p[i].sum;
        }
    else
        {
        if ( i > x )
            {
            x = i;
            return TRUE;
            }
        }

    }

if ( nexti != x )
    {
    x = nexti;
    searchsum = nextsum;
    return TRUE;
    }

return FALSE;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                           TOTAL_SECONDS                              *
***********************************************************************/
int32 DOWNTIME_SUM_CLASS::total_seconds( void )
{
int32 total;
TIME_CLASS t;

total = 0;

if ( p )
    {
    t.get_local_time();
    if ( t > end )
        t = end;
    total = t - start;
    }

return total;
}

/***********************************************************************
*                         DOWNTIME_SUM_CLASS                           *
*                            DOWN_SECONDS                              *
***********************************************************************/
int32 DOWNTIME_SUM_CLASS::down_seconds( void )
{
int32 total;
int32 i;

total = 0;

if ( p )
    {
    for ( i=0; i<n; i++ )
        {
        if ( lstrcmp(p[i].cat, DOWNCAT_SYSTEM_CAT) != 0 || lstrcmp(p[i].subcat, DOWNCAT_UP_SUBCAT) != 0 )
            total += p[i].sum;
        }
    }

return total;
}

/***********************************************************************
*                           MAX_TITLE_PIXELS                           *
***********************************************************************/
int max_title_pixels( int cw )
{
TEXT_LEN_CLASS  tmax;
int w;

tmax.init( MyDc );

tmax.check( resource_string(TOTAL_HOURS_STRING) );
tmax.check( resource_string(TOTAL_HOURS_UP_STRING) );
tmax.check( resource_string(TOTAL_HOURS_DOWN_STRING) );
Titles.rewind();
while ( Titles.next() )
    {
    tmax.check( Titles.text() );
    }

w =  cw * (2*DOWNCAT_NAME_LEN + 2);

maxint( w, tmax.width() );

return w;
}

/***********************************************************************
*                               SHOW_TEXT                              *
***********************************************************************/
static void show_text( int x, int y, LPCTSTR s )
{
int slen;

slen = lstrlen( s );
TextOut( MyDc, x, y, s, slen );
}

/***********************************************************************
*                         SHOW_TITLE_AND_HOURS                         *
***********************************************************************/
void show_title_and_hours( int y, TCHAR * cat_number, TCHAR * subcat, int seconds, BOOLEAN need_percent )
{
double hours;
TCHAR buf[MAX_FLOAT_LEN+1];
int   x;

show_text( LeftX, y, category_name(cat_number) );

x = LeftX + HoursX;
x /= 2;

show_text( x, y, subcat );

SetTextAlign( MyDc, TA_TOP | TA_RIGHT );

hours = (double) seconds;
hours /= REAL_SECONDS_PER_HOUR;
show_text( HoursX, y, fixed_string(hours, 4, 2) );

if ( need_percent )
    {

    hours /= TotalHours;
    hours *= 100.0;
    lstrcpy( buf, fixed_string(hours, 4, 2) );
    lstrcat( buf, TEXT("% ") );
    show_text( PercentX, y, buf );
    }

SetTextAlign( MyDc, TA_TOP | TA_LEFT );
}

/***********************************************************************
*                         SHOW_TITLE_AND_HOURS                         *
***********************************************************************/
void show_title_and_hours( int y, TCHAR * s, int seconds, BOOLEAN need_percent )
{
double hours;
TCHAR buf[MAX_FLOAT_LEN+1];

show_text( LeftX, y, s );

SetTextAlign( MyDc, TA_TOP | TA_RIGHT );

hours = (double) seconds;
hours /= REAL_SECONDS_PER_HOUR;
show_text( HoursX, y, fixed_string(hours, 5, 2) );

if ( need_percent )
    {

    hours /= TotalHours;
    hours *= 100.0;
    lstrcpy( buf, fixed_string(hours, 4, 2) );
    lstrcat( buf, TEXT("% ") );
    show_text( PercentX, y, buf );
    }

SetTextAlign( MyDc, TA_TOP | TA_LEFT );
}

/***********************************************************************
*                         SET_SUBCATEGORY_TITLES                       *
***********************************************************************/
void set_subcategory_titles( void )
{
Titles.empty();
if ( Dh.count() > 0 )
    {
    Dh.rewind();
    while ( Dh.next(CurrentCategory.cat) )
        Titles.append( Dh.title() );
    }
}

/***********************************************************************
*                          SET_CATEGORY_TITLES                         *
***********************************************************************/
void set_category_titles( void )
{
Titles.empty();
if ( Cs.count() > 0 )
    {
    Cs.rewind();
    while ( Cs.next() )
        Titles.append( Cs.title() );
    }

}

/***********************************************************************
*                       PAINT_DOWNBAR_DISPLAY                          *
***********************************************************************/
void paint_downbar_display( void )
{
HFONT oldfont;
HFONT myfont;
TEXTMETRIC tm;
RECT border;
RECT r;

STRING_CLASS s;
int ch;
int cw;
int seconds;
int x;
int y;

float dt;
float t;
float total_seconds;

BOOLEAN first_button;

if ( Dh.count() < 1 )
    return;

oldfont = 0;
myfont  = 0;

if ( !Printing )
    {
    oldfont = (HFONT) SelectObject( MyDc, GetStockObject(SYSTEM_FONT) );
    if ( ArialFontName != DefaultArialFontName )
        {
        cw = average_character_width( MyDc );
        myfont = create_font( MyDc, cw, 0, ArialFontName.text() );
        if ( myfont )
            SelectObject( MyDc, myfont );
        }
    }

GetTextMetrics( MyDc, &tm );

ch = tm.tmHeight - tm.tmInternalLeading;
cw = tm.tmAveCharWidth;

s = resource_string( DOWNBAR_TITLE_STRING );
s += DowntimeMachineName;

if ( CurrentCategory != NoCategory && !Config.show_all_subs )
    {
    s += resource_string( CATEGORY_TITLE_STRING );
    s += category_name( CurrentCategory.cat );
    }

SetTextAlign( MyDc, TA_TOP | TA_CENTER );
SetTextColor( MyDc, CurrentColor[TEXT_COLOR] );
SetBkMode(    MyDc, TRANSPARENT );

x = ( PlotWindowRect.left + PlotWindowRect.right )/2;
y = PlotWindowRect.top+2;

show_text( x, y, s.text() );

if ( Dh.count() < 1 )
    return;

LeftX    = PlotWindowRect.left + cw;
HoursX   = LeftX + max_title_pixels(cw) + 2*cw + pixel_width( MyDc, resource_string(MAX_DOWNTIME_HOURS_STRING) );
PercentX = HoursX + 2*cw + pixel_width( MyDc, resource_string(MAX_DOWNTIME_PERCENT_STRING) );

s = resource_string( DOWNTIME_FROM_STRING );
s += Dh.start.text();
s += resource_string( DOWNTIME_TO_STRING );
s += Dh.end.text();

y += ch + 2;

show_text( x, y, s.text() );

SetTextAlign( MyDc, TA_TOP | TA_LEFT );

y += ch * 2;

SetTextAlign( MyDc, TA_TOP | TA_RIGHT );
show_text( HoursX, y, resource_string(HOURS_STRING) );
SetTextAlign( MyDc, TA_TOP | TA_LEFT );

y += ch * 2;

/*
-----------
Total Hours
----------- */
show_title_and_hours( y, resource_string(TOTAL_HOURS_STRING), Dh.total_seconds(), FALSE );
seconds = Dh.total_seconds();
if ( seconds != 0 )
    {
    TotalHours = seconds;
    TotalHours /= REAL_SECONDS_PER_HOUR;
    total_seconds  = (float) seconds;
    }
else
    {
    TotalHours = 1.0;
    total_seconds  = 1.0;
    }

y += ch;

/*
--------
Hours up
-------- */
seconds = Dh.seconds( DOWNCAT_SYSTEM_CAT, DOWNCAT_UP_SUBCAT );
show_title_and_hours( y, resource_string(TOTAL_HOURS_UP_STRING), seconds, TRUE );
show_text( PercentX+cw, y, resource_string(MACH_UTILIZATION_STRING) );

y += ch;

/*
----------
Hours Down
---------- */
seconds = Dh.down_seconds();
show_title_and_hours( y, resource_string(TOTAL_HOURS_DOWN_STRING), seconds, TRUE );

y += ch;
dt = (float) PlotWindowRect.right - PercentX - 2;

border.top    = PlotWindowRect.bottom;
border.bottom = PlotWindowRect.top;
border.left   = PercentX;
border.right  = PlotWindowRect.right - 1;

GetTextMetrics( MyDc, &tm );

if ( CurrentCategory == NoCategory )
    {
    /*
    ----------------------
    Show the category sums
    ---------------------- */
    first_button = TRUE;
    Titles.rewind();
    Cs.rewind();
    while ( Cs.next() )
        {
        Titles.next();

        x = Cs.seconds();
        y += ch;
        if ( y+ch > PlotWindowRect.bottom )
            break;

        if ( first_button )
            {
            TopButtonY   = y;
            ButtonHeight = ch;
            first_button = FALSE;
            }

        show_title_and_hours( y, Titles.text(), x, TRUE );
        if ( x > 0 )
            {
            r.top = y + tm.tmInternalLeading;
            r.left = PercentX+ 2;
            t = dt;
            t *= (float) x;
            t /= total_seconds;
            r.right = r.left + (int) t;
            r.bottom = y + ch;

            if ( r.bottom > border.bottom )
                border.bottom = r.bottom;

            if ( r.top < border.top )
                border.top = r.top;

            fill_rectangle( MyDc, r, AlarmColor );
            }
        }
    }
else
    {
    /*
    -------------------------------------------
    Show the subcategory sums for this category
    ------------------------------------------- */
    Titles.rewind();

    Cs.rewind();
    while ( Cs.next() )
        {
        Dh.rewind();
        while ( true )
            {
            if ( !Config.show_all_subs )
                {
                if ( !Dh.next(CurrentCategory.cat) )
                    break;
                Titles.next();
                }
            else if ( Config.sort_subs_by_cat )
                {
                if ( !Dh.next(Cs.category()) )
                    break;
                }
            else
                {
                if ( !Dh.next() )
                    break;
                }
            if ( Dh == UpCode )
                continue;

            x = Dh.seconds();

            if ( x == 0 )
                continue;

            y += ch;
            if ( y+ch > PlotWindowRect.bottom )
                break;

            if ( !Config.show_all_subs )
                show_title_and_hours( y, Titles.text(), x, TRUE );
            else
                show_title_and_hours( y, Dh.category(), Dh.title(), x, TRUE );

            if ( x > 0 )
                {
                r.top = y + tm.tmInternalLeading;
                r.left = PercentX+ 2;
                t = dt;
                t *= (float) x;
                t /= total_seconds;
                r.right = r.left + (int) t;
                r.bottom = y + ch;

                if ( r.bottom > border.bottom )
                    border.bottom = r.bottom;

                if ( r.top < border.top )
                    border.top = r.top;

                fill_rectangle( MyDc, r, AlarmColor );
                }
            }
         if ( !Config.show_all_subs || !Config.sort_subs_by_cat )
            break;
        }
    }

x = 3;
if ( PlotWindowRect.right > 1024 )
    x = 10;

border.top    -= x;
border.bottom += x;
border.left--;
border.right++;

draw_rectangle( MyDc, border, CurrentColor[TEXT_COLOR] );

if ( !Printing )
    {
    SelectObject( MyDc, oldfont );
    if ( myfont )
        {
        DeleteObject( myfont );
        myfont = 0;
        }
    }
}

/***********************************************************************
*                             FILL_TIMES                               *
***********************************************************************/
BOOLEAN fill_times( void )
{
BOOLEAN    firstime;
BOOLEAN    past_end;
unsigned long u;
TCHAR    * filename;
DB_TABLE   t;
DOWNTIME_RECORD_CLASS newrec;
DOWNTIME_RECORD_CLASS oldrec;

CAT_BASE_CLASS mycat( NO_DOWNCAT_NUMBER );

filename = downtime_dbname(DowntimeComputerName, DowntimeMachineName);
if ( !file_exists(filename) )
    return FALSE;

Dh.empty();

if ( Config.range_type & DTR_ALL_RANGE )
    {
    firstime = TRUE;
    }
else
    {
    firstime = FALSE;
    oldrec = NoCode;

    /*
    ---------------------------------------------
    lstrcpy( oldrec.cat, NO_DOWNCAT_NUMBER );
    lstrcpy( oldrec.subcat, NO_DOWNCAT_NUMBER );
    --------------------------------------------*/

    if ( Config.range_type & DTR_START_DATE_RANGE )
        oldrec.tm = Config.start;
    else
        {
        oldrec.tm.get_local_time();
        oldrec.tm.set_time( TEXT("00:00:00") );
        if ( Config.nof_days > 1 )
            {
            u = Config.nof_days - 1;
            u *= SECONDS_PER_DAY;
            oldrec.tm -= u;
            }
        }
    Dh.start = oldrec.tm;
    }

if ( Config.range_type & DTR_END_DATE_RANGE )
    {
    Dh.end = Config.end;
    }

past_end = FALSE;
t.open( filename,  DOWNTIME_RECLEN, PFL );
while( t.get_next_record(NO_LOCK) )
    {
    newrec.get( t );
    if ( firstime )
        {
        oldrec = newrec;
        Dh.start = newrec.tm;
        firstime = FALSE;
        }
    else
        {
        if ( Config.range_type & DTR_END_DATE_RANGE )
            {
            if ( newrec.tm > Dh.end )
                past_end = TRUE;
            }
        if ( newrec.tm <= oldrec.tm )
            {
            lstrcpy( oldrec.cat,    newrec.cat    );
            lstrcpy( oldrec.subcat, newrec.subcat );

            }
        else if ( !past_end && newrec.tm > oldrec.tm )
            {
            if ( oldrec != NoCode )
                Dh.seconds(oldrec.cat, oldrec.subcat) += int32( newrec.tm - oldrec.tm );
            oldrec = newrec;
            }
        }

    if ( past_end )
        break;

    if ( !(Config.range_type & DTR_END_DATE_RANGE) )
        Dh.end = newrec.tm;
    }

t.close();

if ( !firstime )
    {
    newrec = oldrec;
    newrec.tm.get_local_time();

    if ( Config.range_type & DTR_END_DATE_RANGE )
        {
        if ( newrec.tm > Dh.end )
            newrec.tm = Dh.end;
        }

    if ( newrec.tm > oldrec.tm )
        Dh.seconds(oldrec.cat, oldrec.subcat) += int32( newrec.tm - oldrec.tm );
    Dh.end = newrec.tm;
    }
else
    {
    return FALSE;
    }

Cs.cleanup();
Dh.rewind();
while ( Dh.next() )
    {
    if ( Dh != UpCode )
        Cs.seconds(Dh.category()) += Dh.seconds();
    }

Cs.sort();
set_category_titles();

return TRUE;
}

/***********************************************************************
*                        DOWNBAR_LEFT_BUTTON_UP                        *
***********************************************************************/
void  downbar_left_button_up( POINT & p )
{
int32 y;
int32 y2;

if ( CurrentCategory == NoCategory )
    {
    y = TopButtonY;
    Cs.rewind();
    while ( Cs.next() )
        {
        y2 = y + ButtonHeight;
        if ( p.y >= y && p.y < y2 )
            {
            CurrentCategory = Cs.category();
            set_subcategory_titles();
            InvalidateRect( MainPlotWindow, 0, TRUE );
            break;
            }
        y = y2;
        }
    }
else
    {
    CurrentCategory = NoCategory;
    set_category_titles();
    InvalidateRect( MainPlotWindow, 0, TRUE );
    }
}

/***********************************************************************
*                            DO_DOWNBAR_BUTTON                         *
***********************************************************************/
void do_downbar_button( int id )
{
if ( id == ESCAPE_KEY )
    {
    CurrentCategory = NoCategory;
    set_category_titles();
    InvalidateRect( MainPlotWindow, 0, TRUE );
    }
}

/***********************************************************************
*                            END_DOWNBAR_DISPLAY                       *
***********************************************************************/
void end_downbar_display( void )
{
Cs.cleanup();
Dh.cleanup();
Titles.empty();
CurrentCategory = NoCategory;
}

/***********************************************************************
*                         BEGIN_DOWNBAR_DISPLAY                        *
***********************************************************************/
BOOLEAN begin_downbar_display( void )
{
if ( *DowntimeComputerName == NullChar )
    copystring( DowntimeComputerName, ComputerName );

if ( *DowntimeMachineName == NullChar )
    copystring( DowntimeMachineName,  MachineName );

Config.read();

if ( Dh.count() < 1 )
    Dh.read();

if ( !fill_times() )
    SendMessage( ChooseDowntimeMachineWindow, WM_DBINIT, 0, 0L );

return TRUE;
}

/***********************************************************************
*                            DO_NEW_DTR_CONFIG                         *
***********************************************************************/
void do_new_dtr_config( void )
{
end_downbar_display();
begin_downbar_display();
InvalidateRect( MainPlotWindow, 0, TRUE );
}