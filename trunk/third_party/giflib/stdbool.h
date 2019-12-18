/*
stdbool.h 
 
-- Boolean type and values
(substitute for missing C99 standard header)
 
public-domain implementation from [EMAIL PROTECTED]
 
implements subclause 7.16 of ISO/IEC 9899:1999 (E)
*/
 
#ifndef __bool_true_false_are_defined
#define __bool_true_false_are_defined 1
 
/* program is allowed to contain its own definitions, so ... */
#undef bool
#undef true
#undef false
 
#if __STDC_VERSION__ < 199901
typedef int _Bool;
#endif
 
#define bool _Bool
#define true 1
#define false 0
 
#endif /* !defined(__bool_true_false_are_defined) */
