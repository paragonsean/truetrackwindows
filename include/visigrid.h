#ifndef _VISI_GRID_
#define _VISI_GRID_

class VISI_GRID_CLASS : public CUGCtrl
{
private:

int     my_font_index;
HWND    prev_control;
HWND    next_control;
int     char_height;
int     char_width;

void    createfont();

public:

BOOLEAN use_row_numbers;

VISI_GRID_CLASS();
~VISI_GRID_CLASS();
void    delete_row();
void    init( int nof_cols, long nof_rows );
void    insert_row( BOOLEAN append );
void    insert_row() { insert_row(FALSE); }
void    OnCharDown( UINT *vcKey, BOOL processed );
void    OnKeyDown( UINT *vcKey, BOOL processed );
int     OnEditContinue(int oldcol,long oldrow,int* newcol,long* newrow);
void    OnLClicked( int col, long row, int updn, RECT * rect, POINT * point, int processed );
void    OnSetup();
void    set_col_width(int col, int width_in_chars );
void    set_neighbors( HWND prev_control_handle, HWND next_control_handle );
void    set_row_numbers();
};

#endif

