#include <windows.h>

#include "..\include\visiparm.h"
#include "..\include\subs.h"
#include "..\include\bitclass.h"

const static int32 BITS_PER_UNSIGNED_INT = sizeof(unsigned int) * 8;

/***********************************************************************
*                               CLEANUP                                *
***********************************************************************/
void BIT_CLASS::cleanup( void )
{

if ( ip )
    {
    delete[] ip;
    ip = 0;
    nof_ints = 0;
    nof_bits = 0;
    }
}

/***********************************************************************
*                               BIT_CLASS                              *
*                                 INIT                                 *
***********************************************************************/
bool BIT_CLASS::init( int32 max_bits, unsigned int * sorc )
{
int32 n;
 
cleanup();
 
if ( max_bits <= 0 )
    return false;
 
/*
-----------------------------------------------------------------------
Calculate the number of unsigned integers I need to hold this many bits
----------------------------------------------------------------------- */
n = max_bits/BITS_PER_UNSIGNED_INT;
if ( max_bits%BITS_PER_UNSIGNED_INT > 0 )
    n++;
 
/*
-------------------------------------------------------
Allocate memory for the array and clear all of the bits
------------------------------------------------------- */
ip = new unsigned int[n];
if ( ip )
    {
    nof_ints = n;
    nof_bits = max_bits;
    for ( n=0; n<nof_ints; n++ )
        {
        if ( sorc )
            ip[n] = sorc[n];
        else
            ip[n] = 0;
        }
 
    return true;
    }
else
    {
    cleanup();
    }
 
return false;
}
 
/***********************************************************************
*                               BIT_CLASS                              *
***********************************************************************/
BIT_CLASS::BIT_CLASS( int32 max_bits )
{
ip       = 0;
nof_ints = 0;
nof_bits = 0;

init( max_bits, 0 );
}
 
/***********************************************************************
*                               BIT_CLASS                              *
***********************************************************************/
BIT_CLASS::BIT_CLASS()
{
ip       = 0;
nof_ints = 0;
nof_bits = 0;
}
 
/***********************************************************************
*                              ~BIT_CLASS                             *
***********************************************************************/
BIT_CLASS::~BIT_CLASS()
{
cleanup();
}

/***********************************************************************
*                               OPERATOR[]                             *
***********************************************************************/
bool BIT_CLASS::operator[]( int32 bit_to_check )
{
bool  state;
int32 int_number;
int32 bit_number;
unsigned int i;

state = false;

if ( ip && bit_to_check >= 0 && bit_to_check < nof_bits )
    {

    int_number = bit_to_check / BITS_PER_UNSIGNED_INT;
    bit_number = bit_to_check % BITS_PER_UNSIGNED_INT;

    i = ip[int_number];

    if ( bit_number )
        i>>=bit_number;

    if ( i & 1 )
        state = true;
    }

return state;
}

/***********************************************************************
*                                 SET                                  *
***********************************************************************/
void BIT_CLASS::set( int32 bit_to_set )
{
int32 int_number;
int32 bit_number;
unsigned int i;

if ( ip && bit_to_set >= 0 && bit_to_set < nof_bits )
    {
    int_number = bit_to_set / BITS_PER_UNSIGNED_INT;
    bit_number = bit_to_set % BITS_PER_UNSIGNED_INT;

    i = 1;
    if ( bit_number )
        i<<=bit_number;

    ip[int_number] |= i;
    }

}
 
/***********************************************************************
*                                RESTORE                               *
***********************************************************************/
bool BIT_CLASS::restore( int bytesize, unsigned int * sorc )
{
int32 n;
 
if ( bytesize < 1 || !sorc )
    return false;
 
cleanup();
 
n = (int32) bytesize;
n /= sizeof( unsigned int );
n -= 2;
 
if ( n != (int32) sorc[0] )
    return false;
 
n = (int32) sorc[1];
init( n, sorc+2 );
 
return true;
}
 
/***********************************************************************
*                                 SAVE                                 *
***********************************************************************/
int BIT_CLASS::save( unsigned int ** dest )
 
{
unsigned int * cp;
int            i;
 
if ( nof_ints < 1 || !ip )
    return 0;
 
*dest = new unsigned int[nof_ints+2];
if ( !(*dest) )
    return 0;
 
cp = *dest;
 
*cp = nof_ints;
cp++;
*cp = nof_bits;
 
for ( i=0; i<nof_ints; i++ )
    {
    cp++;
    *cp = ip[i];
    }
 
i  = nof_ints + 2;
i *= sizeof( unsigned int );
 
return i;
}
 
/***********************************************************************
*                               CLEARALL                               *
***********************************************************************/
void BIT_CLASS::clearall( void )
{
int i;
 
if ( !ip )
    return;
 
for ( i=0; i<nof_ints; i++ )
    ip[i] = 0;
 
}
 
/***********************************************************************
*                                CLEAR                                 *
***********************************************************************/
void BIT_CLASS::clear( int32 bit_to_clear )
{
int32 int_number;
int32 bit_number;
unsigned int i;

if ( ip && bit_to_clear >= 0 && bit_to_clear < nof_bits )
    {
    int_number = bit_to_clear / BITS_PER_UNSIGNED_INT;
    bit_number = bit_to_clear % BITS_PER_UNSIGNED_INT;

    i = 1;
    if ( bit_number )
        i<<=bit_number;

    ip[int_number] &= ~i;
    }

}
