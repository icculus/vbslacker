#/*
# * Intel 80386 ASM routines for ON EVENT support in BASIClib.
# *
# * Ryan is NOT an Assembly guru. Nothing here is guaranteed to be
# * "correct", "clean," or even "well thought out" code.
# *
# * Pass this thing through GCC.
# *   gcc -Wall -c -o OnEventsAsm.o OnEvents.S
# *
# * add -DWIN32 to set up identifiers like cygwin32 expects them.
# *
# * Since this code relies on the compiler doing specific things with the
# *  base pointer, DO NOT EVER COMPILE CODE THAT USES THIS WITH
# *  -fomit-frame-pointer! Be wary of other optimizations, too.
# *
# * Enjoy.
# *
# *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
# */



#ifdef WIN32     /* make symbols friendly for cygwin32... */
#define basePtrIndexes          _basePtrIndexes
#define onEventsLock            _onEventsLock
#define __getCurrentThreadIndex ___getCurrentThreadIndex
#define __obtainThreadLock      ___obtainThreadLock
#define __releaseThreadLock     ___releaseThreadLock
#define __calcBasePtrStorage    ___calcBasePtrStorage
#define __callOnEventHandler    ___callOnEventHandler
#define __resumeNextHandler     ___resumeNextHandler
#define printf                  _printf
#endif


#
# Here's the Bitch Queen Procedure herself. Let's do the nasty...
#

.global __callOnEventHandler

# This procedure copies the stack from event handling procedure to top
#  of stack, calls event handler, and deals with returns. Neet, huh?
#
# The stack is copied to fool the called address into thinking it is
#  executing as normal, with access to all variables, both arguments and
#  local, as they were originally. Before calling the handler, we patch
#  the copied stack so the return address will be in this procedure. This
#  allows us to clean up and return the stack to its normal state. Also,
#  This allows us to "skip" any functions called between the handler
#  procedure and where the event was triggered, so their stack space is not
#  overwritten. We need to protect that data in case of a RESUME NEXT.
#
# The proper element in basePtrIndexes is decremented to allow memory
#  shrinkage. See comments for basePtrStacks declaration in OnEvents.c
#  for a more detailed explanation as to why.
#
# This does not preserve any registers, since we are going to be hopping
#  down the stack to previous procedures that want THEIR preserved
#  registers returned. That is taken care of by other code.
#
#     params : POnEventHandler of handler to call at [esp + 4].
#    returns : any return value from BASIC routine in EAX.

__callOnEventHandler:
        pushl   %ebp                    # Save this for RESUME NEXT calls.
        movl    %esp,%ebp               # Move stack ptr to base ptr.

        call    __calcBasePtrStorage    # Return value is in eax.
        movl    %ebp,(%eax)             # Save base pointer into basePtrStacks.

        movl    8(%ebp),%ebx            # Store POnEventHandler in ebx.

            # Since the C code gives the pOnEventHandler->stackStart as
            #  (&lastArg + sizeof(lastArg)), it is actually ONE past the start
            #  of the stack (since the sizeof would offset us to the next
            #  variable. This is okay, since the following subtraction would
            #  gives us a byte count of the stack that was one short if we
            #  had the honest-to-god start of the stack address...

        movl    4(%ebx),%ecx            # Store ebx->stackStart in ecx.
        movl    8(%ebx),%eax            # Store ebx->stackEnd in eax.
        subl    %eax,%ecx               #  ...and subtract to get stack size.

        movl    4(%ebx),%esi            # Put ebx->stackStart in esi.
        decl    %esi                    # -1 to point to actual start of stack.
        movl    %esp,%edi               # Copy stack pointer to edi.
        decl    %edi                    # Move edi past our stack data.

        subl    %ecx,%esp               # Bump esp for copy space...
        std                             # make lodsb/stosb decrement pointers.
                                        #  ...remember stack goes downward.
stackcopy:                              # loop to move stack...
        lodsb                           # al = *((char *) esi); esi--;
        stosb                           # *((char *) edi) = al; edi--;
        decl    %ecx                    # LOOP replacement. GAS hates it.
        cmpl    $0,%ecx                 # end of loop?
        jnz     stackcopy               #  ...no? Keep going.
#endstackcopy

        movl    12(%ebx),%ebp           # Store ebx->basePtr in ebp.
        incl    %esi                    # point esi to end of copied stack.
        subl    %esp,%esi               # Calculate offset between two stacks.
        subl    %esi,%ebp               #  ...offset ebp to spoof handler.

        movl    $returnloc,4(%ebp)      # patch return addr on stack.


            # Execute BASIC error handler. Any return value is stored in
            #  edx:eax (for 64-bit values, less register space is used for
            #  smaller values.) When the code returns to @returnloc (remember
            #  we patched the stack?), assume all registers to be clobbered,
            #  Yet, we need to preserve all of them for the calling
            #  function. Whew.

        jmp     *(%ebx)                 #  jump blindly into ebx->handlerAddr.

returnloc:                              #  ...and (maybe) land right here.

        pushl   %ebp                    # Save all important registers.
        pushl   %edi
        pushl   %esi
        pushl   %ebx
        pushl   %edx
        pushl   %eax

        call    __calcBasePtrStorage    # Get pointer to our original ebp.
        movl    (%eax),%ebp             # Move it back into ebp.
        movl    8(%ebp),%ebx            # Get POnEventHandler ptr again.

        call    __decrementBPIndexes

        movl    12(%ebx),%ecx           # Store ebx->basePtr in ecx.
        addl    $4,%ecx                 # Offset four points to return addr.

        popl    %eax                    # Restore all important registers.
        popl    %edx
        popl    %ebx
        popl    %esi
        popl    %edi
        popl    %ebp

        movl    %ecx,%esp               # adjust stack for return call...
        ret                             #  ...and pray for the best.
#__callOnEventHandler    endp



# We need to decrement the count of base pointers stored up
#  by these procedures for the current thread. So we find where it is
#  stored, and alter the value...

__decrementBPIndexes:                   # proc
        pushal                          # save everything.

        movl    $onEventsLock,%eax      # Get a lock on Thread-sensitive data.
        pushl   %eax
        call    __obtainThreadLock
        addl    $4,%esp

        call    __getCurrentThreadIndex # Get current thread index in eax.

        movl    $4,%ebx                 # 32-bits; sizeof (void *) == 4.
        mull    %ebx                    # ebx*eax make it into offset in array.

        movl    basePtrIndexes,%esi     # Get array of bp indexes in stacks.
        addl    %eax,%esi               # esi now points to our element.
        movl    (%esi),%eax             # get value pointed to by esi.
        decl    %eax                    # decrement it.
        movl    %eax,(%esi)             # store it back in addr esi points to.

        movl    $onEventsLock,%eax      # Drop lock on Thread-sensitive data.
        pushl   %eax
        call    __releaseThreadLock
        addl    $4,%esp

        popal                           # restore everything.
        ret                             # return.
#__decrementBPIndexes    endp



.global __resumeNextHandler 

#
# This procedure implements RESUME NEXT functionality. Simply,
#  it grabs the base pointer that __callOnEventHandler saved, and gets the
#  needed stack pointer from there. Then, it does some clean up that
#  __callOnEventHandler would do if it was returned to, and then returns.
#  The return point should be in __triggerOnEvent() (see OnEvents.c), on
#  the stack at the point of the latest call to that function. In short,
#  it is like returning from __callOnEventHandler, but we go back to where
#  the OnEvent was triggered, instead of the function that called the
#  function with the error handler. Yikes. That is pretty complex for five
#  OpCodes.  :)
#
#     params : void.
#    returns : void. No registers set for return.
#

__resumeNextHandler:                    # proc
        call    __calcBasePtrStorage    # get ptr to original ebp in eax.
        movl    (%eax),%esp             #  ...and restore it to esp.
        popl    %ebp                    # Reset ebp to sane state.
#        call    __decrementBPIndexes    # Cleanup other data.
        ret                             # Perform miracles.
# __resumeNextHandler   endp


# end of OnEvents.asm ...

