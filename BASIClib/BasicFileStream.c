/*
 * Internal stream support for use with FileIOFunctions.c module.
 *
 *  Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
 */

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include "BasicFileStream.h"

/*** Private variables ***/
#define MAX_FILE_HANDLES    512
static __PBasicFileStream __fileNumbers[MAX_FILE_HANDLES];

/*** File stream functions ***/
boolean __invalidFileNumber(short fileNumber)
/*
 *  Checks if the specified fileNumber is out of range.
 *
 *   params : fileNumber    ==  Any 16-bit value
 *  returns : true if 'fileNumber' is a valid file number.
 */
{
    return (fileNumber < 1 || fileNumber > 511);
}

__PBasicFileStream __getFileStream(short fileNumber)
/*
 *  Returns a pointer to file stream data.  If the file stream structure
 *   hasn't been created, the return value is NULL.
 *
 *   params : fileNumber    ==  Any number that represents an valid file number.
 *  returns : See description
 */
{   
    if(__invalidFileNumber(fileNumber))
        return NULL;
                                    /* Return a pointer to file stream data. */
                                    /*  This will be NULL if the file stream */
                                    /*  is closed. */
    return __fileNumbers[fileNumber];
}

boolean __deleteFileStream(short fileNumber)
/*
 *  De-allocates memory associated with the fileNumber.
 *
 *   params : fileNumber    ==  Any number that represents an open file number.
 *  returns : true if successful, otherwise false.
 */
{
                                    /* Temporary file stream pointer */
    __PBasicFileStream pFileStream;

    pFileStream = __getFileStream(fileNumber);

                                    /* If invalid file stream */
    if(pFileStream == NULL)
        return false;   

                                    /* Free pathname string in structure */
    __freeString(pFileStream->pathName);
                                    /* Free structure memory */
    free(pFileStream);
                                    /* Set pointer to NULL indicating it */
                                    /*  is free */
    __fileNumbers[fileNumber] = NULL;

    return true;
}

__PBasicFileStream __createFileStream(short fileNumber)
/*
 *  Allocates memory associated with the fileNumber.
 *
 *   params : fileNumber    ==  Any number that represents an open file number.
 *  returns : Newly created BasicFileStream structure.  If error, returns NULL.
 */
{
                                    /* Temporary file stream pointer */
    __PBasicFileStream pFileStream;

    if(__invalidFileNumber(fileNumber))
        return NULL;
                                    /* Allocate memory for a new stream */
    pFileStream = (__PBasicFileStream)malloc(sizeof(__BasicFileStream));

                                    /* There was an error allocating memory */
    if(pFileStream == NULL)
        return NULL;

    pFileStream->fileStream = NULL; /* Indicates that stream is CLOSED */

                                    /* Save newly allocated pointer */
    __fileNumbers[fileNumber] = pFileStream;
    
    return pFileStream;             /* Return pointer to new structure */
}

void __initBasicFileStream(void)
/*
 *  Initializes the file stream array if it hasn't already been initialized.
 *
 *   params : void
 *  returns : void
 */
{
                                    /* Reset entire array to NULL pointers */
    memset(__fileNumbers, 0, MAX_FILE_HANDLES * sizeof(__PBasicFileStream));
}
/*** End File stream functions ***/
