/*
 * Routines for internal BASIClib runtime error handling.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "BasicError.h"


#warning !!! basicErrno and __basicErrno need thread-proofing!

/*
 * We keep an extra copy of the BASIC error number around. Both are set
 *  equally in case of an error, but otherwise, "__basicErrno" is
 *  ignored. This allows internal BASIClib routines to reset the errno to
 *  NO_ERROR, call something, and test for error, all without altering
 *  the errno that is accessable by outside programs.
 */
int basicErrno = ERR_NO_ERROR;
int __basicErrno = ERR_NO_ERROR;

    /*
     * These are the strings for ERROR$()...
     */
static char *errStrings[MAX_ERRS];

    /*
     * module-scope ThreadLock.
     */
static ThreadLock basicErrorLock;

    /*
     * onErrorThreadStates stores the start of the handler list used by each
     *  thread. This is an array of pointers, so we only have to use
     *  ThreadLocks when accessing this dynamic array, and not the individual
     *  structures.
     */
static __POnErrorHandler *onErrorThreadStates = NULL;


static void __initErrorStringTable(void);


void __initBasicError(void)
/*
 * This is called once at program startup for initialization purposes.
 *
 *    params : void.
 *   returns : void.
 */
{
    __createThreadLock(&basicErrorLock);
    __initErrorStringTable();
} /* __initErrorFunctions */


void __deinitBasicError(void)
/*
 * This is called once at program termination.
 *  We use this to destroy our ThreadLock.
 *
 *    params : void.
 *   returns : void.
 */
{
    __destroyThreadLock(&basicErrorLock);
} /* __deinitBasicError */


void __initThreadBasicError(int tidx)
/*
 * This makes sure space exists in the thread-protected arrays for the
 *  current thread index. This is called whenever a new thread is created.
 *
 * OBTAIN THE THREADLOCK BEFORE EVER TOUCHING (onErrorThreadStates)!
 *
 *      params : void.
 *     returns : void, but all the tables could get realloc()ed.
 */
{
    __obtainThreadLock(&basicErrorLock);
    onErrorThreadStates = realloc(onErrorThreadStates,
                                  (__getHighestThreadIndex() + 1) *
                                      sizeof (__POnErrorHandler *));

    if (onErrorThreadStates == NULL)
        __fatalRuntimeError(ERR_INTERNAL_ERROR);

    onErrorThreadStates[tidx] = NULL;
    __releaseThreadLock(&basicErrorLock);
} /* __initThreadOnError */




/*
 * There is no __deinitThreadBasicError(), since all the data we would free
 *  is all just stored on the stack of functions that need On Error support.
 *
 * The only thing we would need to do is set an element in onErrorThreadStates
 *  to NULL, which __initThreadOnError() will do anyhow if the thread index is ever
 *  ever used again.
 */




static __POnErrorHandler __getOnErrorThreadState(void)
/*
 * Get a pointer to start of current thread's linked list of handlers. This
 *  takes care of most of the thread-proofing details.
 *
 *     params : void.
 *    returns : Pointer to start of current thread's linked list of handlers.
 */
{
    __POnErrorHandler retVal;

    __obtainThreadLock(&basicErrorLock);
    retVal = onErrorThreadStates[__getCurrentThreadIndex()];
    __releaseThreadLock(&basicErrorLock);

    return(retVal);
} /* __getOnErrorThreadState */


__boolean __isOnErrorThreadStateNULL(void)
/*
 * This is for debugging purposes; is the list of handlers for
 *  this thread clear or not?
 *
 *     params : void.
 *    returns : boolean (true) if handler list empty, (false) otherwise.
 */
{
    __POnErrorHandler list = __getOnErrorThreadState();
    return((list == NULL) ? true : false);
} /* __isOnErrorThreadStateNULL */


static void __setOnErrorThreadState(__POnErrorHandler pHandler)
/*
 * Set the start of current thread's linked list of handlers. This
 *  takes care of most of the thread-proofing details.
 *
 *     params : pHandler == Pointer to handler you want to start list.
 *    returns : void.
 */
{
    __obtainThreadLock(&basicErrorLock);
    onErrorThreadStates[__getCurrentThreadIndex()] = pHandler;
    __releaseThreadLock(&basicErrorLock);
} /* __setOnErrorThreadState */



void __registerOnErrorHandler(__POnErrorHandler pHandler, void *handlerAddr)
/*
 * This is somewhere between "low level but general" and "80x386 specific."
 *
 * Ideally, any module that contains a call to this function should
 *  do something like this:
 *
 *  #include "StdBasic.h"
 *
 *  void myFunc(void)
 *  {
 *      __ONErrorSUPPORT;
 *                !!! write this.
 *      __register
 *  }
 *
 * handlerAddr is the goto label we'll be blindly jumping to to handle the
 *  the runtime error. NULL is like ON ERROR GOTO 0.
 *
 * evType just guarantees that we don't call a timer handler for a runtime
 *  error, etc...
 *
 *    (*whew!*)   !!! rewrite all of this!
 *
 * All that said, call this function to register an error handler with
 *  BASIClib that will be called when appropriate. Ideally, the parser/compiler
 *  will generate all the calls to this function when it finds ON ERROR GOTO
 *  commands.
 *
 * Only one error handler of each type can be active for any given procedure.
 *  This function will check the handler to see if it should replace a
 *  previous one, based on base pointers, which will be constant for all
 *  calls from an instance of a single procedure.
 *
 *     returns : void.
 */
{
    __POnErrorHandler oldStartOfList;

        /*
         * Determine if we should replace a currently registered handler.
         *  This is determined by whether any handlers exist for the
         *  specified base pointer. If that condition is true, then we are
         *  replacing a previous handler.
         *
         * Handlers are ordered in list starting with the most recent
         *  base pointer. If the first handler != (base), then
         *  this base pointer has not registered an error handler.
         */

    oldStartOfList = __getOnErrorThreadState();

    if (oldStartOfList != pHandler)         /* setup new handler? */
    {
        __setOnErrorThreadState(pHandler);
        pHandler->next = oldStartOfList;
    } /* if */

        /*
         * Final setup...note that a lot of this was handled by the
         *  macros you should have used. You DID use them, didn't you?
         */
    pHandler->handlerAddr = handlerAddr;
    pHandler->isActive = false;
} /* __registerOnErrorHandler */


void __deregisterOnErrorHandler(__POnErrorHandler pHandler)
/*
 * Call this to deregister (pHandler).
 *
 * Do not call directly; please call the __exitCleanupOnError macro
 *  (defined in OnError.h)
 *
 *      params : void.
 *     returns : void.
 */
{
    if (__getOnErrorThreadState() == pHandler)
        __setOnErrorThreadState(pHandler->next);
} /* __deregisterOnErrorHandler */


static __POnErrorHandler __getOnErrorHandler(void)
/*
 * Returns the current ON ERROR GOTO handler. Ignores handlers that are
 *  currently active.
 *
 * If we hit an error handler address for the specified type that is NULL,
 *  then we stop looking, since this signifies an ON ERROR GOTO 0 command,
 *  which disables handling for that OnError.
 *
 * All handlers registered to points above the stack from the chosen handler
 *  (if a handler is chosen) are deregistered, since they will go out of
 *  scope when the chosen handler is activated. Even activated ones go away,
 *  in this case.
 *
 *    params : evType == type of error we want handler for.
 *   returns : Most significant handler. NULL if there isn't one, or
 *             handling has been disabled, or something.
 */
{
    __POnErrorHandler retVal;
    __POnErrorHandler list; 
    __POnErrorHandler pHandler;

    pHandler = list = __getOnErrorThreadState();

    while ((pHandler != NULL) && (retVal == NULL))
    {
        if (pHandler->isActive == false)
            retVal = pHandler;
        pHandler = pHandler->next;
    } /* while */

        /*
         * "On Error Goto 0" handlers are never "active", so code will fall
         *  here if we stumble across one...but we need to check for it...
         *  these are signified by a NULL in the handlerAddr field, and if
         *  this is the case, there is no appropriate handler for the error,
         *  since "On Error Goto 0" disables error trapping for the scope of
         *  that procedure and above.
         *
         * If the handler is STILL acceptable after this check, it is moved to
         *  the first position in the list, and all handlers registered to
         *  points above the stack from the handler are removed. This means
         *  that handlers are ordered in a first-in-last-out manner (or
         *  perhaps this is better stated "last-triggered-first-listed"), so
         *  when we hit a RESUME, we know the handler it belongs to is the
         *  first (active) handler listed. (When we drop out of a given base
         *  pointer, those handlers are deregistered, so the start of the list
         *  are always the most current base pointer's handler.)
         */
    if (retVal->handlerAddr == NULL)    /* NULL == On Error Goto 0. */
        retVal = NULL;
    else                                /* move to start of list... */
    {
        for (; list != retVal; list = pHandler) /* ditch out-of-scope stuff */
        {
            pHandler = list->next;
            free(pHandler);
        } /* for */

        __setOnErrorThreadState(retVal);
    } /* else */

    return(retVal);
} /* __getOnErrorHandler */


static __boolean __triggerOnError(void)
/*
 * This function activates the correct OnError handler.
 *  Will only call a handler if it a) exists, and b) is not currently active.
 *
 * __setStackAndJump is a macro defined in Assembler.h ...
 *
 *    params : void.
 *   returns : either won't return directly, due to stack voodoo, or if no
 *              Error handler of specified type is available, returns (false).
 */
{
    __POnErrorHandler pHandler = __getOnErrorHandler();

    if (pHandler == NULL)    /* No handler available?  Bail out. */
        return(false);

    pHandler->isActive = true;

    __setStackAndJump(pHandler->basePtr,
                      pHandler->stackPtr,
                      pHandler->handlerAddr);

    return(true);            /* should never hit this, but just in case... */
} /* __triggerOnError */


void __prepareResume(void *base)
/*
 * Basic's RESUME command support. A (probably fatal) runtime error is
 *  thrown if you call RESUME when no handlers are active for a given point
 *  on the stack. If a handler is active, it is set to be NOT active, and
 *  we return. Presumably, then the macro that should have called this will
 *  perform an unconditional jump to somewhere...
 *
 * DO NOT CALL THIS FUNCTION DIRECTLY. Please use the macro __resumeNext,
 *  __resumeZero, or __resumeLabel(addr) (defined in OnError.h).
 *
 *     params : void.
 *    returns : Should never return, as code jumps elsewhere.
 */
{
    __POnErrorHandler pHandler = __getOnErrorThreadState();

    if ((pHandler->basePtr != base) || (pHandler->isActive == false))
        __runtimeError(ERR_RESUME_WITHOUT_ERROR);
    else
        pHandler->isActive = false;
} /* __prepareResume */


static void __defaultRuntimeErrorHandler(void)
{
#warning update __defaultRuntimeErrorHandler()!
#ifdef BROKEN
    char *errStr;
    char msg[strlen(errStr) + 300];   /* !!! generalize? */

    errStr = ((basicErrno > MAX_ERRS) ?
                STR_UNKNOWN_ERR : errStrings[basicErrno]);

    if (errStr == NULL)
        errStr = STR_UNKNOWN_ERR;

/* !!! */
    sprintf(msg, "\n\n***Unhandled runtime error***\n"
                 "  \"%s\" (#%d)\n"
                 "    - __stIP     == (%p)\n"
                 "    - __stNextIP == (%p)\n"
                 "    - __stBP     == (%p)\n"
                 "    - __stSP     == (%p)\n"
                 "\n\n",
                 errStr, basicErrno, __stIP, __stNextIP, __stBP, __stSP);

    __printAsciz(msg);
#endif
    exit(basicErrno);
} /* __defaultRuntimeErrorHandler */


void __fatalRuntimeError(int errorNum)
/*
 * Call this instead of __runtimeError() if you want to throw an
 *  unrecoverable error. Even ERR_NO_ERROR is fatal here.
 *
 *    params : errorNum == error number to throw.
 *   returns : never.
 */
{
    basicErrno = __basicErrno = errorNum;
    __defaultRuntimeErrorHandler();
} /* __fatalRuntimeError */


void __runtimeError(int errorNum)
{
    basicErrno = __basicErrno = errorNum;

    if (errorNum != ERR_NO_ERROR)
    {
        if (__triggerOnError() == false)
            __defaultRuntimeErrorHandler();
    } /* if */
} /* __runtimeError */


static void __initErrorStringTable(void)
/* !!! comment */
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
    errStrings[ERR_PERMISSION_DENIED] = STR_PERMISSION_DENIED;
    errStrings[ERR_DISK_NOT_READY] = STR_DISK_NOT_READY;
    errStrings[ERR_DISK_MEDIA_ERROR] = STR_DISK_MEDIA_ERROR;
    errStrings[ERR_FEATURE_UNAVAILABLE] = STR_FEATURE_UNAVAILABLE;
    errStrings[ERR_RENAME_ACROSS_DISKS] = STR_RENAME_ACROSS_DISKS;
    errStrings[ERR_PATH_FILE_ACCESS_ERROR] = STR_PATH_FILE_ACCESS_ERROR;
    errStrings[ERR_PATH_NOT_FOUND] = STR_PATH_NOT_FOUND;
} /* __initErrorStringTable */

/* end of BasicError.c ... */

