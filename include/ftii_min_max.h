#ifndef _FTII_MIN_MAX_DATA_
#define _FTII_MIN_MAX_DATA_

class FTII_MIN_MAX_DATA {
    public:
    int   curve_type;
    union { int imin; unsigned int umin; };
    union { int imax; unsigned int umax; };
    FTII_MIN_MAX_DATA() { curve_type = FTII_NO_CURVE_TYPE; imin=0; imax=0; }
    ~FTII_MIN_MAX_DATA() {}
    void operator=( const FTII_MIN_MAX_DATA & sorc ) { curve_type=sorc.curve_type; umin=sorc.umin; umax=sorc.umax; }
    };

#endif
