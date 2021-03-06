/*
 * Internal vbSlacker runtime error stuff.
 *
 *   Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 */

#ifndef _INCLUDE_STDBASIC_H_

#include "StdBasic.h"

#else

#ifndef _INCLUDE_BASICERROR_H_
#define _INCLUDE_BASICERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Runtime error codes.
 *
 * !!! These are only some of the ones in VB6.0 ...there's a buttload more.
 */

#define ERR_NO_ERROR                       0
#define ERR_RETURN_WITHOUT_GOSUB           3
#define ERR_OUT_OF_DATA                    4
#define ERR_ILLEGAL_FUNCTION_CALL          5
#define ERR_OVERFLOW                       6
#define ERR_OUT_OF_MEMORY                  7
#define ERR_SUBSCRIPT_OUT_OF_RANGE         9
#define ERR_ARRAY_FIXED_OR_LOCKED          10
#define ERR_DIVISION_BY_ZERO               11
#define ERR_TYPE_MISMATCH                  13
#define ERR_OUT_OF_STRING_SPACE            14
#define ERR_EXPRESSION_TOO_COMPLEX         16
#define ERR_CANNOT_CONTINUE                17
#define ERR_USER_INTERRUPT_OCCURRED        18
#define ERR_RESUME_WITHOUT_ERROR           20
#define ERR_OUT_OF_STACK_SPACE             28
#define ERR_SUBPROGRAM_NOT_DEFINED         35
#define ERR_TOO_MANY_DLL_APP_CLIENTS       47
#define ERR_ERROR_IN_LOADING_DLL           48
#define ERR_BAD_DLL_CALLING_CONVENTION     49
#define ERR_INTERNAL_ERROR                 51
#define ERR_BAD_FILE_NAME_OR_NUMBER        52
#define ERR_FILE_NOT_FOUND                 53
#define ERR_BAD_FILE_MODE                  54
#define ERR_FILE_ALREADY_OPEN              55
#define ERR_FIELD_STATEMENT_ACTIVE         56
#define ERR_DEVICE_IO_ERROR                57
#define ERR_FILE_ALREADY_EXISTS            58
#define ERR_BAD_RECORD_LENGTH              59
#define ERR_DISK_FULL                      61
#define ERR_INPUT_PAST_END_OF_FILE         62
#define ERR_BAD_RECORD_NUMBER              63
#define ERR_TOO_MANY_FILES                 67
#define ERR_DEVICE_UNAVAILABLE             68
#define ERR_COMMUNICATION_BUFFER_OVERFLOW  69
#define ERR_PERMISSION_DENIED              70
#define ERR_DISK_NOT_READY                 71
#define ERR_RENAME_ACROSS_DISKS            74
#define ERR_PATH_FILE_ACCESS_ERROR         75
#define ERR_PATH_NOT_FOUND                 76

typedef struct _ONERRORHANDLER
{
    void *thisInstruction;
    void *nextInstruction;
    void *handlerAddr;             /* Code address of handler.             */
    void *basePtr;                 /* Base pointer in handler's function.  */
    void *stackPtr;                /* Stack pointer in handler's function. */
    __boolean isActive;            /* Is handler active?                   */
    struct _ONERRORHANDLER *next;  /* linked list stuff.                   */
} __OnErrorHandler;

typedef __OnErrorHandler *__POnErrorHandler;


/* function prototypes... */

void __initBasicError(void);
void __deinitBasicError(void);
void __initThreadBasicError(__integer tidx);

__boolean __isOnErrorThreadStateNULL(void);
void __runtimeError(__long errorNum);
void __fatalRuntimeError(__long errorNum);
void __registerOnErrorHandler(__POnErrorHandler pHandler);
void __deregisterOnErrorHandler(__POnErrorHandler pHandler);
void __prepareResume(__POnErrorHandler pHandler);
__long __getBasicErrno(void);

#define __ONERRORVARS      __OnErrorHandler __onError = {NULL,  \
                                                         NULL,  \
                                                         NULL,  \
                                                         NULL,  \
                                                         NULL,  \
                                                         false, \
                                                         NULL}
                                                          

#define __ONERRORINIT()    __getBasePointer(__onError.basePtr);   \
                           __getStackPointer(__onError.stackPtr)


#define __setOnErrorHandler(addr) __getLabelAddr(addr, __onError.handlerAddr);\
                                  __registerOnErrorHandler(&__onError)

#define __setOnErrorHandlerOff()  __registerOnErrorHandler(NULL)

#define __setInstructs(iThis, iNext)  if (!__onError.isActive)              \
                                      {                                     \
                                         __getLabelAddr(iThis,              \
                                                __onError.thisInstruction); \
                                         __getLabelAddr(iNext,              \
                                                __onError.nextInstruction); \
                                      }

#define __resumeNext()           __prepareResume(&__onError);  \
                                 __jump(__onError.nextInstruction)

#define __resumeZero()           __prepareResume(&__onError);  \
                                 __jump(__onError.thisInstruction)

#define __resumeLabel(addr)      __prepareResume(&__onError);  \
                                 __jumpLabel(addr)

#define __exitCleanupOnError()   __deregisterOnErrorHandler(&__onError)

#define __insertResumeNextHandler(label)  __insertLineLabel(label); \
                                          __resumeNext()

#ifdef __cplusplus
}
#endif

#endif  /* _INCLUDE_BASICERROR_H_ */
#endif  /* _INCLUDE_STDBASIC_H_   */

/* end of BasicError.h ... */

