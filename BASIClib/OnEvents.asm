;/*
; * Intel 80386 ASM routines for ON EVENT support in BASIClib.
; *
; * Ryan is NOT an Assembly guru. Nothing here is guaranteed to be
; * "correct", "clean," or even "well thought out" code.
; *
; * Under i386, ELF-based Linux, we used NASM to assemble this code into
; *  an object file that we can link into BASIClib. I only know enough
; *  about assembly to be confused when writing in AT&T syntax, so GAS was
; *  out of the question. A good command line for assembling is this:
; *
; *     nasm -o OnEventsAsm.o -f elf OnEvents.asm
; *
; * NASM can be found, in all it's ANSI C source code glory, at !!!URL.
; *  It can produce all sorts of Intel-based object file formats, such as
; *  ELF, a.out, win32, 16-bit DOS, etc...AND it compiled without a
; *  complaint on both Linux and Win95 with cygwin32.
; *
; * Enjoy.
; *
; *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
; */


;
; This points to an array of elements by thread index. This array
;  contains ints that tell how many base pointers we've stacked up on
;  recursive error handling. This procedure needs to decrement it when
;  the error has been handled.

extern basePtrIndexes

;
; Other external stuff...
;

extern __getCurrentThreadIndex         ; See Threads.c ...
extern __enterCriticalThreadSection    ; See Threads.c ...
extern __exitCriticalThreadSection     ; See Threads.c ...
extern __calcBasePtrStorage            ; See OnEvents.c ...


;
; Here's the Bitch Queen Procedure herself. Let's do the nasty...
;

global __callOnEventHandler

; This procedure copies the stack from event handling procedure to top
;  of stack, calls event handler, and deals with returns. Neet, huh?
;
; The stack is copied to fool the called address into thinking it is
;  executing as normal, with access to all variables, both arguments and
;  local, as they were originally. Before calling the handler, we patch
;  the copied stack so the return address will be in this procedure. This
;  allows us to clean up and return the stack to its normal state. Also,
;  This allows us to "skip" any functions called between the handler's
;  procedure and where the event was triggered, so their stack space isn't
;  overwritten. We need to protect that data in case of a RESUME NEXT.
;
; The proper element in basePtrIndexes is decremented to allow memory
;  shrinkage. See comments for basePtrStacks's declaration in OnEvents.c
;  for a more detailed explanation as to why.
;
; This doesn't preserve any registers, since we are going to be hopping
;  down the stack to previous procedures that want THEIR preserved
;  registers returned. That is taken care of by other code.
;
;     params : POnEventHandler of handler to call at [esp + 4].
;    returns : any return value from BASIC routine in EAX.

__callOnEventHandler:                   ; proc
        push    ebp
        mov     ebp,esp                 ; Move stack ptr to base ptr.

        call    __calcBasePtrStorage    ; Return value is in eax.
        mov     dword [eax],ebp         ; Save base pointer into basePtrStacks.

        mov     ebx,[ebp + 8]           ; Store POnEventHandler in ebx.

            ; Since the C code gives the pOnEventHandler->stackStart as
            ;  (&lastArg + sizeof(lastArg)), we're actually ONE past the start
            ;  of the stack (since the sizeof would offset us to the next
            ;  variable. This is okay, since the following subtraction would
            ;  gives us a byte count of the stack that was one short if we
            ;  had the honest-to-god start of the stack address...

        mov     ecx,[ebx + 4]           ; Store ebx->stackStart in ecx.
        mov     eax,[ebx + 8]           ; Store ebx->stackEnd in eax.
        sub     ecx,eax                 ;  ...and subtract to get stack size.

        mov     esi,[ebx + 4]           ; Put ebx->stackStart in esi.
        dec     esi                     ; -1 to point to actual start of stack.
        mov     edi,esp                 ; Copy stack pointer to edi.
        dec     edi                     ; Move edi past our stack data.

        sub     esp,ecx                 ; Bump esp for copy space...
        std                             ; make lodsb/stosb decrement pointers.
                                        ;  ...remember stack goes downward.
@stackcopy:                             ; loop to move stack...
        lodsb                           ; al = *((char *) esi); esi--;
        stosb                           ; *((char *) edi) = al; edi--;
        loop    @stackcopy              ; ecx--; if (ecx != 0) goto @stackcopy;
@endstackcopy:

        mov     ebp,[ebx + 12]          ; Store ebx->basePtr in ebp.
        inc     esi                     ; point esi to end of copied stack.
        sub     esi,esp                 ; Calculate offset between two stacks.
        sub     ebp,esi                 ;  ...offset ebp to spoof handler.

        mov     dword [ebp + 4], @returnloc    ; patch return addr on stack.


            ; Execute BASIC error handler. Any return value is stored in
            ;  edx:eax (for 64-bit values, less register space is used for
            ;  smaller values.) When the code returns to @returnloc (remember
            ;  we patched the stack?), assume all registers to be clobbered,
            ;  Yet, we need to preserve all of them for the calling
            ;  function. Whew.

        jmp     [ebx]                   ; ...jump blindly into ebx->handlerAddr.

@returnloc:                             ;  ...and (maybe) land right here.

        push    ebp                     ; Save all important registers.
        push    edi
        push    esi
        push    ebx
        push    edx
        push    eax

        call    __calcBasePtrStorage    ; Get pointer to our original ebp.
        mov     ebp,[eax]               ; Move it back into ebp.
        mov     ebx,[ebp + 8]           ; Get POnEventHandler ptr again.

        call    __decrementBPIndexes

        mov     ecx,[ebx + 12]          ; Store ebx->basePtr in esi.
        add     ecx,4                   ; Offset four points to return addr.

        pop     eax                     ; Restore all important registers.
        pop     edx
        pop     ebx
        pop     esi
        pop     edi
        pop     ebp

        mov     esp,ecx                 ; adjust stack for return call...
        ret                             ;  ...and pray for the best.
;__callOnEventHandler    endp



; We need to decrement the count of base pointers stored up
;  by these procedures for the current thread. So we find where it's
;  stored, and alter the value...

__decrementBPIndexes:                   ; proc
        pushad                          ; Save everything.
        call    __getCurrentThreadIndex ; Get current thread index in eax.
        call    __enterCriticalThreadSection
        mov     ebx,4                   ; 32-bits; sizeof (void *) == 4.
        mul     ebx                     ; ebx*eax make it into offset in array.
        mov     esi,[basePtrIndexes]    ; Get array of bp indexes in stacks.
        add     esi,eax                 ; esi now points to our element.
        mov     eax,[esi]               ; get value pointed to by esi.
        dec     eax                     ; decrement it.
        mov     dword [esi],eax         ; store it back in addr esi points to.
        call    __exitCriticalThreadSection
        popad                           ; restore everything.
        ret                             ; return.
;__decrementBPIndexes    endp



global __resumeNextHandler

;
; This procedure implements BASIC's RESUME NEXT functionality. Simply,
;  it grabs the base pointer that __callOnEventHandler saved, and gets the
;  needed stack pointer from there. Then, it does some clean up that
;  __callOnEventHandler would do if it was returned to, and then returns.
;  The return point should be in __triggerOnEvent() (see OnEvents.c), on
;  the stack at the point of the latest call to that function. In short,
;  it's like returning from __callOnEventHandler, but we go back to where
;  the OnEvent was triggered, instead of the function that called the
;  function with the error handler. Yikes. That's pretty complex for five
;  OpCodes.  :)
;
;     params : void.
;    returns : void. No registers set for return.
;

__resumeNextHandler:                    ; proc
        call    __calcBasePtrStorage    ; get ptr to original ebp in eax.
        mov     esp,[eax]               ;  ...and restore it to esp.
        pop     ebp                     ; Reset ebp to sane state.
        call    __decrementBPIndexes    ; Cleanup other data.
        ret                             ; Perform miracles.
;__resumeNextHandler    endp


; end of OnEvents.asm ...

