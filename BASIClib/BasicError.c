/*
 * Routines for internal BASIClib runtime error handling.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include "BasicError.h"


/* basicErrno and __basicErrno need thread-proofing! */

/*
 * We keep an extra copy of the BASIC error number around. Both are set
 *  equally in case of an error, but otherwise, "__basicErrno" is
 *  ignored. This allows internal BASIClib routines to reset the errno to
 *  NO_ERROR, call something, and test for error, all without altering
 *  the errno that is accessable by outside programs.
 */
int basicErrno = ERR_NO_ERROR;
int __basicErrno = ERR_NO_ERROR;

static char *errStrings[MAX_ERRS];

void __initErrorFunctions(STATEPARAMS)
/*
 * Initialize the table of errorStrings.
 *
 *  !!! write this later. Internationalization should be considered, eh?
 */
{
    memset(errStrings, '\0', sizeof (errStrings));
    errStrings[ERR_NO_ERROR] = STR_NO_ERROR;
    errStrings[ERR_NEXT_WITHOUT_FOR] = STR_NEXT_WITHOUT_FOR;
    errStrings[ERR_SYNTAX_ERROR] = STR_SYNTAX_ERROR;
    errStrings[ERR_RETURN_WITHOUT_GOSUB] = STR_RETURN_WITHOUT_GOSUB;
    errStrings[ERR_OUT_OF_DATA] = STR_OUT_OF_DATA;
    errStrings[ERR_ILLEGAL_FUNCTION_CALL] = STR_ILLEGAL_FUNCTION_CALL;
    errStrings[ERR_OVERFLOW] = STR_OVERFLOW;
    errStrings[ERR_OUT_OF_MEMORY] = STR_OUT_OF_MEMORY;
    errStrings[ERR_LABEL_NOT_DEFINED] = STR_LABEL_NOT_DEFINED;
    errStrings[ERR_SUBSCRIPT_OUT_OF_RANGE] = STR_SUBSCRIPT_OUT_OF_RANGE;
    errStrings[ERR_DUPLICATE_DEFINITION] = STR_DUPLICATE_DEFINITION;
    errStrings[ERR_DIVISION_BY_ZERO] = STR_DIVISION_BY_ZERO;
    errStrings[ERR_ILLEGAL_IN_DIRECT_MODE] = STR_ILLEGAL_IN_DIRECT_MODE;
    errStrings[ERR_TYPE_MISMATCH] = STR_TYPE_MISMATCH;
    errStrings[ERR_OUT_OF_STRING_SPACE] = STR_OUT_OF_STRING_SPACE;
    errStrings[ERR_STRING_FORMULA_TOO_COMPLEX]=STR_STRING_FORMULA_TOO_COMPLEX;
    errStrings[ERR_CANNOT_CONTINUE] = STR_CANNOT_CONTINUE;
    errStrings[ERR_FUNCTION_NOT_DEFINED] = STR_FUNCTION_NOT_DEFINED;
    errStrings[ERR_NO_RESUME] = STR_NO_RESUME;
    errStrings[ERR_RESUME_WITHOUT_ERROR] = STR_RESUME_WITHOUT_ERROR;
    errStrings[ERR_DEVICE_TIMEOUT] = STR_DEVICE_TIMEOUT;
    errStrings[ERR_DEVICE_FAULT] = STR_DEVICE_FAULT;
    errStrings[ERR_FOR_WITHOUT_NEXT] = STR_FOR_WITHOUT_NEXT;
    errStrings[ERR_OUT_OF_PAPER] = STR_OUT_OF_PAPER;
    errStrings[ERR_WHILE_WITHOUT_WEND] = STR_WHILE_WITHOUT_WEND;
    errStrings[ERR_WEND_WITHOUT_WHILE] = STR_WEND_WITHOUT_WHILE;
    errStrings[ERR_DUPLICATE_LABEL] = STR_DUPLICATE_LABEL;
    errStrings[ERR_SUBPROGRAM_NOT_DEFINED] = STR_SUBPROGRAM_NOT_DEFINED;
    errStrings[ERR_ARGUMENT_COUNT_MISMATCH] = STR_ARGUMENT_COUNT_MISMATCH;
    errStrings[ERR_ARRAY_NOT_DEFINED] = STR_ARRAY_NOT_DEFINED;
    errStrings[ERR_VARIABLE_REQUIRED] = STR_VARIABLE_REQUIRED;
    errStrings[ERR_FIELD_OVERFLOW] = STR_FIELD_OVERFLOW;
    errStrings[ERR_INTERNAL_ERROR] = STR_INTERNAL_ERROR;
    errStrings[ERR_BAD_FILE_NAME_OR_NUMBER] = STR_BAD_FILE_NAME_OR_NUMBER;
    errStrings[ERR_FILE_NOT_FOUND] = STR_FILE_NOT_FOUND;
    errStrings[ERR_BAD_FILE_MODE] = STR_BAD_FILE_MODE;
    errStrings[ERR_FILE_ALREADY_OPEN] = STR_FILE_ALREADY_OPEN;
    errStrings[ERR_FIELD_STATEMENT_ACTIVE] = STR_FIELD_STATEMENT_ACTIVE;
    errStrings[ERR_DEVICE_IO_ERROR] = STR_DEVICE_IO_ERROR;
    errStrings[ERR_FILE_ALREADY_EXISTS] = STR_FILE_ALREADY_EXISTS;
    errStrings[ERR_BAD_RECORD_LENGTH] = STR_BAD_RECORD_LENGTH;
    errStrings[ERR_DISK_FULL] = STR_DISK_FULL;
    errStrings[ERR_INPUT_PAST_END_OF_FILE] = STR_INPUT_PAST_END_OF_FILE;
    errStrings[ERR_BAD_RECORD_NUMBER] = STR_BAD_RECORD_NUMBER;
    errStrings[ERR_BAD_FILE_NAME] = STR_BAD_FILE_NAME;
    errStrings[ERR_TOO_MANY_FILES] = STR_TOO_MANY_FILES;
    errStrings[ERR_DEVICE_UNAVAILABLE] = STR_DEVICE_UNAVAILABLE;
    errStrings[ERR_COMMUNICATION_BUFFER_OVERFLOW] =
               STR_COMMUNICATION_BUFFER_OVERFLOW;
    errStrings[ERR_PERMISSION_DENIED] = STR_ERR_PERMISSION_DENIED;
    errStrings[ERR_DISK_NOT_READY] = STR_DISK_NOT_READY;
    errStrings[ERR_DISK_MEDIA_ERROR] = STR_DISK_MEDIA_ERROR;
    errStrings[ERR_FEATURE_UNAVAILABLE] = STR_FEATURE_UNAVAILABLE;
    errStrings[ERR_RENAME_ACROSS_DISKS] = STR_RENAME_ACROSS_DISKS;
    errStrings[ERR_PATH_FILE_ACCESS_ERROR] = STR_PATH_FILE_ACCESS_ERROR;
    errStrings[ERR_PATH_NOT_FOUND] = STR_PATH_NOT_FOUND;
} /* __initErrorFunctions */


void __defaultRuntimeErrorHandler(STATEPARAMS)
{
    char *errStr;

    errStr = ((basicErrno > MAX_ERRS) ? UNKNOWN_ERR : errStrings[basicErrno]);
    if (errStr == NULL)
        errStr = UNKNOWN_ERR;

    /* !!! this needs to get abstracted! */

    printf("\n\n***Unhandled runtime error***\n"
           "  \"%s\" (#%d)\n"
           "    - __stIP     == (%p)\n"
           "    - __stNextIP == (%p)\n"
           "    - __stBP     == (%p)\n"
           "    - __stSP     == (%p)\n"
           "\n\n",
            errStr, basicErrno, __stIP, __stNextIP, __stBP, __stSP);

    exit(basicErrno);
} /* __defaultRuntimeErrorHandler */


void __fatalRuntimeError(STATEPARAMS, int errorNum)
/*
 * Call this instead of __runtimeError() if you want to throw an
 *  unrecoverable error. Even ERR_NO_ERROR is fatal here.
 *
 *    params : errorNum == error number to throw.
 *   returns : never.
 */
{
    basicErrno = __basicErrno = errorNum;
    __defaultRuntimeErrorHandler(STATEARGS);
} /* __fatalRuntimeError */



void __runtimeError(STATEPARAMS, int errorNum)
{
    POnEventHandler pHandler;

    basicErrno = __basicErrno = errorNum;

    if (errorNum != ERR_NO_ERROR)
    {
        pHandler = __getOnEventHandler(STATEARGS, ONERROR);
        if ((pHandler == NULL) || (pHandler->handlerAddr == NULL))
            __defaultRuntimeErrorHandler(STATEARGS);
        else
            __triggerOnEvent(STATEARGS, pHandler, ONERROR);
    } /* if */
} /* __runtimeError */


/* end of BasicError.c ... */

