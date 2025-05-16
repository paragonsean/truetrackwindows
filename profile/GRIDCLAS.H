#include "..\include\ugctrl.h"

const int AVERAGE_COLUMN_NUMBER    = 2;
const int LOW_ALARM_COLUMN_NUMBER  = 5;
const int HIGH_ALARM_COLUMN_NUMBER = 6;
const int LCL_COLUMN_NUMBER        = 7;
const int UCL_COLUMN_NUMBER        = 8;
const int ZMIN_COLUMN_NUMBER       = 11;

class GRID_CLASS : public CUGCtrl
{

private:
static HFONT my_default_font;

public:

GRID_CLASS();
~GRID_CLASS();

void createfont( void );

void OnSetup();

void OnLClicked( int col, long row, int updn, RECT *rect, POINT *point, BOOL processed);
void OnSH_LClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed );
int OnEditStart( int col, long row, HWND *edit);
int OnEditFinish( int col, long row, HWND edit, LPCTSTR string, BOOL cancelFlag );
void GRID_CLASS::OnCharDown( UINT *vcKey, int processed );
};
