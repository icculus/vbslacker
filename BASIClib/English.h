/*
 * Header file for ENGLISH string literals in BASIClib.
 *
 *  Copyright (c) 1999 Ryan C. Gordon and Gregory S. Read.
 *   This text written by Ryan C. Gordon.
 */

/*
 * If you are translating to a new language, do this:
 *
 *  1. Ask yourself, "Do I have a good grasp of C and BASIC to understand
 *     this file, and what it's purpose is? Do I have sufficient translating
 *     skills? Do I have the ethics to NOT put naughty words that the
 *     BASIClib maintainers will not understand in here?" If you answered
 *     "no" to any of these questions, stop now.
 *  2. Change the "This text written by" line above to contain your
 *     name. DO NOT CHANGE THE COPYRIGHT. You will be given credit for your
 *     work, but by submitting your work, the maintainers of BASIClib gain
 *     sole ownership of it.
 *  3. Change any occurence of the word "ENGLISH" to the language you
 *     have translated this file to.
 *  4. Don't change the identifiers. For example, you should not
 *     change STR_NO_ERROR, but you SHOULD translate "No error" ...
 *     Exceptions are noted below.
 *  5. Rename the file to fit the language, like "German.h", or whatever.
 *     We'd appreciate if you named the file by the english version of your
 *     language. That is, name it "Spanish.h", not "Espanol.h". Gracias.
 *  6. E-mail your completed work to ryan_gordon@hotmail.com. He'll
 *     incorporate it into the next release of BASIClib.
 *  7. Expect to be notified if we have more translating work for you
 *     in the future.  :)
 *
 *                                   Have fun,
 *                                      --ryan.
 */


/*
 * Change both occurances of the below constant to _INCLUDE_MYLANGUAGE_H_ ...
 *  that is, _INCLUDE_SPANISH_H_ or _INCLUDE_GREEK_H_ or whatever...
 */
#ifndef _INCLUDE_ENGLISH_H_
#define _INCLUDE_ENGLISH_H_


/*
 * This first batch are error messages that might be encountered
 *  during the course of a BASIC program's execution.
 */
#define STR_NO_ERROR                        "No error"
#define STR_NEXT_WITHOUT_FOR                "NEXT without FOR"
#define STR_SYNTAX_ERROR                    "Syntax error"
#define STR_RETURN_WITHOUT_GOSUB            "RETURN without GOSUB"
#define STR_OUT_OF_DATA                     "Out of data"
#define STR_ILLEGAL_FUNCTION_CALL           "Illegal Function call"
#define STR_OVERFLOW                        "Overflow"
#define STR_OUT_OF_MEMORY                   "Out of memory"
#define STR_LABEL_NOT_DEFINED               "Label not defined"
#define STR_SUBSCRIPT_OUT_OF_RANGE          "Subscript out of range"
#define STR_DUPLICATE_DEFINITION            "Duplicate Definition"
#define STR_DIVISION_BY_ZERO                "Division by zero"
#define STR_ILLEGAL_IN_DIRECT_MODE          "Illegal in direct mode"
#define STR_TYPE_MISMATCH                   "Type mismatch"
#define STR_OUT_OF_STRING_SPACE             "Out of string space"
#define STR_STRING_FORMULA_TOO_COMPLEX      "String formula too complex."
#define STR_CANNOT_CONTINUE                 "Cannot continue"
#define STR_FUNCTION_NOT_DEFINED            "Function not defined"
#define STR_NO_RESUME                       "No resume"
#define STR_RESUME_WITHOUT_ERROR            "Resume without error"
#define STR_DEVICE_TIMEOUT                  "Device timeout"
#define STR_DEVICE_FAULT                    "Device fault"
#define STR_FOR_WITHOUT_NEXT                "FOR without NEXT"
#define STR_OUT_OF_PAPER                    "Out of paper"
#define STR_WHILE_WITHOUT_WEND              "WHILE without WEND"
#define STR_WEND_WITHOUT_WHILE              "WEND without WHILE"
#define STR_DUPLICATE_LABEL                 "Duplicate label"
#define STR_SUBPROGRAM_NOT_DEFINED          "Subprogram not defined"
#define STR_ARGUMENT_COUNT_MISMATCH         "Argument count mismatch"
#define STR_ARRAY_NOT_DEFINED               "Array not defined"
#define STR_VARIABLE_REQUIRED               "Variable required"
#define STR_FIELD_OVERFLOW                  "Field overflow"
#define STR_INTERNAL_ERROR                  "Internal error"
#define STR_BAD_FILE_NAME_OR_NUMBER         "Bad file name or number"
#define STR_FILE_NOT_FOUND                  "File not found"
#define STR_BAD_FILE_MODE                   "Bad file mode"
#define STR_FILE_ALREADY_OPEN               "File already open"
#define STR_FIELD_STATEMENT_ACTIVE          "Field statement active"
#define STR_DEVICE_IO_ERROR                 "Device I/O error"
#define STR_FILE_ALREADY_EXISTS             "File already exists"
#define STR_BAD_RECORD_LENGTH               "Bad record length"
#define STR_DISK_FULL                       "Disk full"
#define STR_INPUT_PAST_END_OF_FILE          "Input past end of file"
#define STR_BAD_RECORD_NUMBER               "Bad record number"
#define STR_BAD_FILE_NAME                   "Bad file name"
#define STR_TOO_MANY_FILES                  "Too many files"
#define STR_DEVICE_UNAVAILABLE              "Device unavailable"
#define STR_COMMUNICATION_BUFFER_OVERFLOW   "Communication buffer overflow"
#define STR_PERMISSION_DENIED               "Permission denied"
#define STR_DISK_NOT_READY                  "Disk not ready"
#define STR_DISK_MEDIA_ERROR                "Disk media error"
#define STR_FEATURE_UNAVAILABLE             "Feature unavailable"
#define STR_RENAME_ACROSS_DISKS             "Rename across disks"
#define STR_PATH_FILE_ACCESS_ERROR          "Path/File access error"
#define STR_PATH_NOT_FOUND                  "Path not found"
#define STR_UNKNOWN_ERR                     "Unknown Error"

#endif   /* don't touch this line. */

/* end of English.h ... */

