//
// File: include/stdlib.h
//
// Description: Some stuff, every C++ program needs
//
//
// Author: Ulf Vatter <uvatter@ira.uka.de>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//


#if !defined(__INCLUDE__STDLIB_H__)
#define __INCLUDE__STDLIB_H__

#include <sdi/types.h>

/*
const class {
 public:
    template<class T> operator T*() const {
	return 0;
    }
    template<class C, class T> operator T C::*() const {
	return 0;
	}
 private:
    void operator&() const;
} NULL;
*/

#define NULL 0

extern "C" inline void memset ( void * dest, char c, size_t size )
    {
    char * d = reinterpret_cast<char*>( dest );

    for ( ;size--; )
        *( d++ ) = c;
    }

extern "C" inline void memcpy ( void * dest, const void * src, size_t size )
    {
    char * d = reinterpret_cast<char*>( dest );
    const char* s = reinterpret_cast<const char*>( src );

    while ( size-- )
        *( d++ ) = *( s++ );
    }

extern "C" inline char* strncpy ( char* dest, const char* src, size_t max )
    {
    while ( max-- )
	{
        if ( !( *( dest++ ) = *( src++ ) ) )
            break;
    }
    while ( max < 0 )
	{
        *( dest++ ) = '\0';
        max--;
    }; // pad with zeros (ansi requires this)
    return dest;
    }

extern "C" inline int strncmp ( const char* dest, const char* src, size_t max )
    {
    while ( ( max ) && ( *dest ) ) {
        if ( *dest != *src )
            break;
        dest++;
        src++;
        max--;
        }
    return (max)?*dest - *src:0;
    }
    
extern "C" inline int strlen ( const char* src )
    {
    const char* start = src;
    while ( *src ) 
    	src++;
    return src - start;
    }
    
extern "C" inline char* strchr ( char* src, const char c )
    {
    while ( *src && *src != c )
    	src++;
    	
    if ( *src != c ) {
    	src = NULL;
    }
    
    return src;
    }

extern "C" inline char* strcpy ( char* dest, const char* src )
    {
    char* start = dest;
    while ((*dest++ = *src++));

    return start;
    }

extern inline word_t min( word_t a, word_t b )
    {
    return (a<b)?a:b;
    }

extern inline word_t max( word_t a, word_t b )
    {
    return (a>b)?a:b;
    }

extern inline word_t around( word_t roundee, word_t rounder )
    {	// Rounder better be a power of 2
    return roundee & ( ~( rounder - 1 ) );
    }

extern inline word_t aroundLb( word_t roundee, word_t rounder )
    {	// Rounder better be a log base 2 of the actual rounder
    return around( roundee, 1UL << rounder );
    }

extern inline word_t aroundUp( word_t roundee, word_t rounder )
    {	// Rounder better be a power of 2
    return around( roundee + ( rounder - 1 ), rounder );
    }

extern inline word_t aroundUpLb( word_t roundee, word_t rounder )
    {	// Rounder better be a log base 2 of the actual rounder
    return aroundUp( roundee, 1UL << rounder );
    }

extern inline word_t msBit( word_t bits )
    {	// Find most significant bit (you'll have to check for yourself wether it has any bits set)
#define ARCH_IA32
#ifdef ARCH_IA32
    word_t res;
    asm (                               // the keyword volatile is not needed for this simple calculation
        "bsr %[in], %[out]       \n" // bit scan reverse
        "jnz 1f                  \n" // If it was not zero, jump to label 1
        "movl $0xFFFFFFFF,%[out] \n" // so it was zero. Than let's return some kind of error code
        "1:                      \n" // This is our label for above
    : [ out ] "=r" ( res )                  // output
                : [ in ] "r" ( bits )                   // input
                // 	:                                  // clobber
            );
    return res;
#else

    register word_t i = ( sizeof( word_t ) * 8 - 1 );
    for ( word_t j = 1UL << ( sizeof( word_t ) * 8 - 1 ); j > 0; j >>= 1, i-- )
        if ( bits & j )
            return i;
    return ~0UL;
#endif
    }

extern inline word_t lsBit( word_t bits )
    {	// Find least significant bit (you'll have to check for yourself wether it has any bits set)
#ifdef ARCH_IA32
    word_t res;
    asm (                               // the keyword volatile is not needed for this simple calculation
        "bsf %[in], %[out]       \n" // bit scan forward
        "jnz 1f                  \n" // If it was not zero, jump to label 1
        "movl $0xFFFFFFFF,%[out] \n" // so it was zero. Than let's return some kind of error code
        "1:                      \n" // This is our label for above
    : [ out ] "=r" ( res )                  // output
                : [ in ] "r" ( bits )                   // input
                // 	:                                  // clobber
            );
    return res;
#else

    register word_t i = 0;
    for ( word_t j = 1UL; j < ( 1UL << ( sizeof( word_t ) * 8 - 1 ) ); j <<= 1, i++ )
        if ( bits & j )
            return i;
    return ~0UL;
#endif
    }

extern inline word_t numBits( word_t bits )
    {
    int ret = 0;
    for ( word_t i = 1UL; i < ( 1UL << ( sizeof( word_t ) * 8 - 1 ) ); i <<= 1 )
        if ( bits & i )
            ret++;
    return ret;
    }

#endif

