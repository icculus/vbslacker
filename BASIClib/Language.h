/*
 * Entry header for language-specific headers. Never directly #include
 *  something like "English.h" or "German.h" ... it's better to
 *  #include "Language.h" and -DLANG_ENGLISH on the command line.
 *
 *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_LANGUAGE_H_
#define _INCLUDE_LANGUAGE_H_


#if defined LANG_ENGLISH
    #include "English.h"
#else
    #error No language defined. Try "-DLANG_ENGLISH" ...
#endif


#endif /* #ifndef INCLUDE_LANGUAGE_H_ */

#endif /* #ifndef INCLUDE_STDBASIC_H_ */

/* end of Language.h ... */

