;/*
; * Intel 80386 ASM routines for ON EVENT GOTO support in BASIClib.
; *
; * Ryan is NOT an Assembly guru. Nothing here is guaranteed to be
; * "correct",
; *  "clean," or even "well thought out" code.
; *
; * Under i386, ELF-based Linux, we used NASM to assemble this code into
; *  an object file that we can link into BASIClib. I only know enough
; *  about assembly to be confused when writing in AT&T syntax, so GAS was
; *  out of the question. A good command line for assembling is this:
; *
; *     nasm -o OnEventsAsm.o -f elf OnEvents.asm
; *
; * Enjoy.
; *
; *    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.
; */



;
; When we execute the error handler, we lose the contents of every
;  register. Most importantly, we lose the base pointer (EBP), which
;  tells us where our parameters and local data can be found on the stack.
;
; The only way to keep a reference to anything in that case is to have it
;  in a global variable, since we always know the location of it. So we keep
;  a (void **) global var, called basePtrStacks, in OnEvents.c. This variable
;  is an array of base pointers, since event handling can stack in a
;  recursive manner. The C code, before calling the event handler,
;  makes sure there's enough room in the array to handle another base
;  pointer, and then calls this assembly code. Before executing the
;  error handler, this assembly procedure will save it's base pointer at the
;  index specified by basePtrIndexes. After the handler, we have to start from
;  scratch, so we retrieve our base pointer from where we saved it, feeling
;  for our digital asshole with both hands and these global variables.
;  Once returned to a sane state, we decrement basePtrIndexes, so that the
;  space used by the stack of base pointers can begin to shrink.
;
; ...of course, this is complicated by thread-protecting our data. :)
;  basePtrStacks is actually a (void ***), so we have something roughly
;  equivalent to basePtrStacks[__getCurrentThreadIndex()] \
;                             [basePtrIndexes[__getCurrentThreadIndex()];
;
; Ugh.

extern basePtrStacks
extern basePtrIndexes

;
; Other external stuff...
;

extern __getCurrentThreadIndex
extern __enterCriticalThreadSection
extern __exitCriticalThreadSection
extern __calcBasePtrStorage

;
; Here's the procedure itself. Let's do the nasty...
;

; !!! lose this. !!!
extern spacer
extern printf
extern printText
extern printTabs

callText   db   '__callOnEventHandler()...',0Ah,0h
survived   db   'Survived error handler. Cleaning up...',0Ah,0h
origEBP    db   'Original EBP == (%p)',0Ah,0h

; !!! lose this. !!!


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
; This doesn't preserve any registers, since we are going to be hopping
;  down the stack to previous procedures that want THEIR preserved
;  registers returned. That is taken care of by other code.
;
;     params : POnEventHandler of handler to call at [esp + 4].
;    returns : any return value from BASIC routine in EAX.

__callOnEventHandler:                   ; proc

             ; !!! Lose this
        pushad
        mov     eax,callText
        push    eax
        mov     eax,[spacer]
        push    eax
        call    printText
        add     esp,8
        popad
        
        pushad
        mov     eax,[spacer]
        inc     eax
        mov     dword [spacer],eax
        popad
            ; !!! Lose this


            ; Due to all our stack voodoo, there's no need to go through
            ;  a few "standard" assembly procedure details, like pushing
            ;  the original base pointer. Therefore, all arguments will be
            ;  at [ebp + 4], NOT [ebp + 8]. Local variables will still be
            ;  at [ebp - 4], etc...

        mov     ebp,esp                 ; Move stack ptr to base ptr.
        sub     esp,4                   ; Allocate local variables...

        call    __calcBasePtrStorage    ; Return value is in edi.        
        mov     dword [eax],ebp         ; Save base pointer.

        mov     ebx,[ebp + 4]           ; Store POnEventHandler in ebx.
        mov     ecx,[ebx + 4]           ; Store ebx->stackStart in ecx.
        mov     eax,[ebx + 8]           ; Store ebx->stackEnd in eax.
        sub     ecx,eax                 ;  ...subtract to get stack size - 1...
        inc     ecx                     ;  ...so add one.

        mov     esi,[ebx + 12]          ; Store ebx->origReturnAddr in ESI.
        mov     eax,[esi]               ; Get value pointed to by esi...
        mov     dword [ebp - 4],eax     ;  ...and save it to local variable...
                                        ;  ...this is where we'll return to.

        mov     dword [esi],@returnloc  ; patch return address on stack.

        mov     esi,[ebx + 4]           ; Put stack start in esi.
        mov     edi,esp                 ; Copy stack pointer to edi.

        sub     esp,ecx                 ; Bump esp for copy space...
        std                             ; make lodsb/stosb decrement pointers.
                                        ;  ...remember stack goes downward.
@stackcopy:                             ; loop to move stack...
        lodsb                           ; al = *((char *) esi); esi--;
        stosb                           ; *((char *) edi) = al; edi--;
        loop    @stackcopy              ; ecx--; if (ecx != 0) goto @stackcopy;
@endstackcopy:

        mov     eax,[ebx + 16]          ; Store ebx->basePtr in eax.

            ; !!! lose this.
        pushad
        mov     ecx,[spacer]
        push    ecx
        call    printTabs
        add     esp,4
        popad

        pushad
        push    eax
        mov     eax,origEBP
        push    eax
        call    printf
        add     esp,8
        popad
            ; !!! lose this.

        dec     esi                     ; point esi to end of copied stack.
        sub     esi,esp                 ; Calculate offset between two stacks.
        add     eax,esi                 ;  ...add it to original base pointer...
        mov     ebp,eax                 ;  ...to fake ebp for handler.

            ; Execute BASIC error handler. Any return value is stored in
            ;  edx:eax (for 64-bit values, less register space is used for
            ;  smaller values.) When the code returns to @returnloc (remember
            ;  we patched the stack?), assume all registers to be clobbered,
            ;  Yet, we need to preserve all of them for the calling
            ;  function. Whew.

        jmp     [ebx]                   ;  ...jump blindly into event handler...

@returnloc:                             ;  ...and (maybe) land right here.

            ; !!! lose this.
        pushad
        mov     ecx,[spacer]
        push    ecx
        call    printTabs
        add     esp,4
        popad

        pushad
        mov     eax,survived
        push    eax
        call    printf
        add     esp,4
        popad
            ; !!! lose this.


        push    ebp                     ; Save all important registers.
        push    edi
        push    esi
        push    ebx
        push    edx
        push    eax

        call    __calcBasePtrStorage    ; Get pointer to our original ebp.
        mov     ebp,[eax]               ; Move it back into ebp.

            ; We need to decrement the count of base pointers stored up
            ;  by this procedure for the current thread. So we find where it's
            ;  stored, and alter the value...

        call    __getCurrentThreadIndex ; Get current thread index.
        call    __enterCriticalThreadSection
        mov     esi,[basePtrIndexes]    ; Get array of bp indexes in stacks.
        mov     ebx,4                   ; 32-bits; sizeof (void *) == 4.
        mul     ebx                     ; ebx*eax make it into offset in array.
        add     esi,eax                 ; esi now points to our element.
        call    __exitCriticalThreadSection

        mov     eax,[esi]               ; get value pointed to by esi.
        dec     eax                     ; decrement it.
        mov     dword [esi],eax         ; store it back in addr esi points to.

        mov     ebx,[ebp + 4]           ; Get POnEventHandler ptr again.

        mov     esi,[ebx + 12]          ; Store ebx->OrigReturnAddr in esi.
        mov     eax,[ebp - 4]           ; Get original value of retaddr in eax.
        mov     dword [esi],eax         ; Repatch stack with original value.

        add     esi,4                   ; origReturnAddr + 4 is esp for ret...
        mov     ecx,esi                 ;  ...and save it in ecx...

        pop     eax                     ; Restore all important registers.
        pop     edx
        pop     ebx
        pop     esi
        pop     edi
        pop     ebp

             ; !!! Lose this
        pushad
        mov     eax,[spacer]
        dec     eax
        mov     dword [spacer],eax

        mov     eax,callText
        push    eax
        mov     eax,[spacer]
        push    eax
        call    printText
        add     esp,8
        popad        
            ; !!! Lose this


        mov     esp,ecx                 ; adjust stack for return call...
        ret                             ;  ...and pray for the best.
;__callOnEventHandler    endp


; end of OnEvents.asm ...

