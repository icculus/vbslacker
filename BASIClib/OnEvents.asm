; Intel 80386 ASM routines for ON EVENT GOTO support in BASIClib.
;
; Ryan is NOT an Assembly guru. Nothing here is guaranteed to be "correct",
;  "clean," or even "well thought out" code.
;
;    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.

                              
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
;  procedure and where the event was triggered.
;
;     params : POnEventHandler of handler to call at [esp + 4].
;              (This will be [ebp + 8] when we set up the base ptr.)
;    returns : any return value from BASIC routine in EAX.

__callOnEventHandler:                   ; proc
        push    ebp                     ; Save calling function's base ptr.
        mov     ebp,esp                 ; Move stack ptr to base ptr.
        sub     esp,4                   ; Allocate local variables...

        push    esi                     ; Save registers...
        push    edi
        push    ebx

        mov     ebx,[ebp + 8]           ; Store POnEventHandler in ebx.

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
        std                             ; set lodsb/stosb to decrement pointers.
                                        ;  ...remember stack goes downward.
@stackcopy:                             ; loop to move stack...
        lodsb                           ; al = *((char *) esi); esi--;
        stosb                           ; *((char *) edi) = al; edi--;
        loop    @stackcopy              ; ecx--;  if (ecx != 0) goto @stackcopy;
@endstackcopy:

        mov     eax,[ebx + 16]          ; Store ebx->basePtr in eax.
        dec     esi                     ; point esi to end of copied stack.
        sub     esi,esp                 ; Calculate offset between two stacks.
        add     eax,esi                 ;  ...add it to original base pointer...
        mov     ebp,eax                 ;  ...to fake ebp for handler.

            ; Execute BASIC error handler. Any return value is stored in
            ;  edx:eax (for 64-bit values, less register space is used for
            ;  smaller values.) When the code returns to @returnloc (remember
            ;  we patched the stack?), esp will be wherever it was before
            ;  we subtracted it before @stackcopy, and we can backtrack from
            ;  there. ebp will have the base pointer of the function that
            ;  called the function containing the error handler. Assume all
            ;  other registers to be clobbered. Whew.

        mov     edi,[ebx]               ; Store ebx->handlerAddr in edi...
        jmp     [edi]                   ;  ...jump blindly into it...

@returnloc:                             ;  ...and (maybe) land right here.

        ; !!! do something.

        pop     ebx                     ; Restore registers...
        pop     edi
        pop     esi

        add     esp,4                   ; Remove local variables.
        mov     esp,ebp                 ; Restore original stack pointer.
        pop     ebp                     ; Restore calling function's base ptr.
        ret                             ; Return.
;__callOnEventHandler    endp

; end of OnEvents.asm ...


