#ifndef _MULTIPART_RUNLIST_CLASS_
#define _MULTIPART_RUNLIST_CLASS_

#ifndef _STRING_CLASS_
#include "..\include\stringcl.h"
#endif

const int MAX_MULTIPART_RUNLIST = 8;

class MULTIPART_RUNLIST_CLASS
{
public:

BOOLEAN is_enabled;
STRING_CLASS partname[MAX_MULTIPART_RUNLIST];

MULTIPART_RUNLIST_CLASS() { is_enabled=FALSE; }
~MULTIPART_RUNLIST_CLASS(){}
BOOLEAN get( TCHAR * cn, TCHAR * mn, TCHAR * pn );
BOOLEAN put( TCHAR * cn, TCHAR * mn, TCHAR * pn );
BOOLEAN create_runlist( TCHAR * co, TCHAR * ma, TCHAR * pa );
};

#endif

