/*
 * Declarations for BASIC runtime error subsystem.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_BASICERROR_H_
#define _INCLUDE_BASICERROR_H_

/*
 * Runtime error codes. Some of these aren't applicable, but have been
 *  left here for history's sake. Most of the ones that aren't applicable
 *  as runtime errors could still be referenced by the compiler, like
 *  NEXT_WITHOUT_FOR.
 *
 * !!! These are from Qbasic...what else has been added since?
 */

typedef enum
{
    NO_ERROR                      = 0,
    NEXT_WITHOUT_FOR              = 1,
    SYNTAX_ERROR                  = 2,
    RETURN_WIHTOUT_GOSUB          = 3,
    OUT_OF_DATA                   = 4,
    ILLEGAL_FUNCTION_CALL         = 5,
    OVERFLOW                      = 6,
    OUT_OF_MEMORY                 = 7,
    LABEL_NOT_DEFINED             = 8,
    SUBSCRIPT_OUT_OF_RANGE        = 9,
    DUPLICATE_DEFINITION          = 10,
    DIVISION_BY_ZERO              = 11,
    ILLEGAL_IN_DIRECT_MODE        = 12,
    TYPE_MISMATCH                 = 13,
    OUT_OF_STRING_SPACE           = 14,
    STRING_FORMULA_TOO_COMPLEX    = 16,
    CANNOT_CONTINUE               = 17,
    FUNCTION_NOT_DEFINED          = 18,
    NO_RESUME                     = 19,
    RESUME_WITHOUT_ERROR          = 20,
    DEVICE_TIMEOUT                = 24,
    DEVICE_FAULT                  = 25,
    FOR_WITHOUT_NEXT              = 26,
    OUT_OF_PAPER                  = 27,
    WHILE_WITHOUT_WEND            = 29,
    WEND_WITHOUT_WHILE            = 30,
    DUPLICATE_LABEL               = 33,
    SUBPROGRAM_NOT_DEFINED        = 35,
    ARGUMENT_COUNT_MISMATCH       = 37,
    ARRAY_NOT_DEFINED             = 38,
    VARIABLE_REQUIRED             = 40,
    FIELD_OVERFLOW                = 50,
    INTERNAL_ERROR                = 51,
    BAD_FILE_NAME_OR_NUMBER       = 52,
    FILE_NOT_FOUND                = 53,
    BAD_FILE_MODE                 = 54,
    FILE_ALREADY_OPEN             = 55,
    FIELD_STATEMENT_ACTIVE        = 56,
    DEVICE_IO_ERROR               = 57,
    FILE_ALREADY_EXISTS           = 58,
    BAD_RECORD_LENGTH             = 59,
    DISK_FULL                     = 61,
    INPUT_PAST_END_OF_FILE        = 62,
    BAD_RECORD_NUMBER             = 63,
    BAD_FILE_NAME                 = 64,
    TOO_MANY_FILES                = 67,
    DEVICE_UNAVAILABLE            = 68,
    COMMUNICATION_BUFFER_OVERFLOW = 69,
    PERMISSION_DENIED             = 70,
    DISK_NOT_READY                = 71,
    DISK_MEDIA_ERROR              = 72,
    FEATURE_UNAVAILABLE           = 73,
    RENAME_ACROSS_DISKS           = 74,
    PATH_FILE_ACCESS_ERROR        = 75,
    PATH_NOT_FOUND                = 76
} RuntimeErrEnum;

typedef RuntimeErrEnum *PRuntimeErrEnum;

void __runtimeError(RuntimeErrEnum errorNum);  /* set and handle errors. */
double err(void);                              /* get BASIC errno.       */

#endif


/* end of ErrorFunctions.h ... */


