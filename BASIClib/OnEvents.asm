; Intel 80386 ASM routines for ON EVENT GOTO support in BASIClib.
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

__callOnEventHandler:                   ; proc near
        push    ebp                     ; Save calling function's base ptr.
        mov     ebp,esp                 ; Move stack ptr to base ptr.

        push    esi                     ; Save registers...
        push    edi
        push    ebx

        mov     ebx,[ebp + 8]           ; Store POnEventHandler in EBX.
        mov     edi,[ebx]               ; Store handlerAddr in EDI.
        mov     edx,[ebx + 8]           ; Store stackEnd in EDX.
        mov     esi,[ebx + 12]          ; Store origReturnAddr in ESI.
        mov     ebx,[ebx + 4]           ; Store stackStart in EBX.

        mov     dword [esi],@returnloc        ; patch return address on stack.

        mov     esi,ebx                 ; Put stack start in ESI.
        mov     ebx,esp                 ; Copy stack pointer to EBX.

        std                             ; decrement on LODSB...
@stackcopy:                             ; loop to move stack...
        cmp     edx,esi                 ; have we hit the end?
        ja      @endstackcopy           ; then stop.
        lodsb                           ; Put byte in AL, decrement ESI.
        mov     [ebx],cl    ; copy a byte.
        dec     esp                     ; point to next byte...
        dec     ebx                     ;   ...remember stack goes downward.
        jmp     @stackcopy              ; do it again.
@endstackcopy:

;
; EAX handlerAddr ... overwritten by callee with retVal.
; EBX copy of base ptr ... preserved by callee.
; ECX not used
; EDX
; ESI
; EDI
; ESP
; EBP
;

        mov     ebx,ebp                 ; Save copy of base pointer.
        mov     ebp,esp                 ; Fake the base ptr for handler.
        jmp     [edi]                   ; execute BASIC error handler.
                                        ;  retVal is in EAX.
@returnloc:

        mov     ebp,ebx                 ; Move base pointer back.

        pop     ebx                     ; Restore registers...
        pop     edi
        pop     esi

        mov     esp,ebp                 ; Restore original stack pointer.
        pop     ebp                     ; Restore calling function's base ptr.
        ret                             ; Return.
;__callOnEventHandler    endp

; end of OnEvents.asm ...


