/*
 * Dynamic language support header.
 *
 *    Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *     Written by Ryan C. Gordon.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_LANGUAGE_H_
#define _INCLUDE_LANGUAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    STR_UNHANDLED_RT_ERROR,
    STR_NO_ERROR,
    STR_RETURN_WITHOUT_GOSUB,
    STR_OUT_OF_DATA,
    STR_ILLEGAL_FUNCTION_CALL,
    STR_OVERFLOW,
    STR_OUT_OF_MEMORY,
    STR_SUBSCRIPT_OUT_OF_RANGE,
    STR_DIVISION_BY_ZERO,
    STR_TYPE_MISMATCH,
    STR_OUT_OF_STRING_SPACE,
    STR_EXPRESSION_TOO_COMPLEX,
    STR_CANNOT_CONTINUE,
    STR_RESUME_WITHOUT_ERROR,
    STR_SUBPROGRAM_NOT_DEFINED,
    STR_INTERNAL_ERROR,
    STR_BAD_FILE_NAME_OR_NUMBER,
    STR_FILE_NOT_FOUND,
    STR_BAD_FILE_MODE,
    STR_FILE_ALREADY_OPEN,
    STR_FIELD_STATEMENT_ACTIVE,
    STR_DEVICE_IO_ERROR,
    STR_FILE_ALREADY_EXISTS,
    STR_BAD_RECORD_LENGTH,
    STR_DISK_FULL,
    STR_INPUT_PAST_END_OF_FILE,
    STR_BAD_RECORD_NUMBER,
    STR_TOO_MANY_FILES,
    STR_DEVICE_UNAVAILABLE,
    STR_COMMUNICATION_BUFFER_OVERFLOW,
    STR_PERMISSION_DENIED,
    STR_DISK_NOT_READY,
    STR_RENAME_ACROSS_DISKS,
    STR_PATH_FILE_ACCESS_ERROR,
    STR_PATH_NOT_FOUND,
    STR_UNKNOWN_ERR,
    _TOTAL_STRING_COUNT_  /* !!! namespace pollution. */
} __BASIClibStringIndex;

extern char *__BASIClibStrings[];

void __initLanguage(char *vbHomeDir);
void __deinitLanguage(void);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef INCLUDE_LANGUAGE_H_ */
#endif /* #ifndef INCLUDE_STDBASIC_H_ */

/* end of Language.h ... */

