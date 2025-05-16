#ifndef _STDDOWN_CLASS_
#define _STDDOWN_CLASS_

struct STDDOWN_ENTRY
{
TCHAR category[DOWNCAT_NUMBER_LEN+1];
TCHAR subcategory[DOWNCAT_NUMBER_LEN+1];
TCHAR startime[ALPHATIME_LEN+1];
TCHAR endtime[ALPHATIME_LEN+1];
};

class STDDOWN_CLASS
{
private:
STDDOWN_ENTRY * p;
int             n;
int             current_index;

public:
STDDOWN_CLASS() { n=0; p = 0; current_index = 0; }
~STDDOWN_CLASS() { cleanup(); }
void cleanup() { if (p) { delete[] p; p=0; n=0; current_index = 0; } }
void startup();
bool find( long time_to_find );
TCHAR * cat();
TCHAR * subcat();
};

#endif
