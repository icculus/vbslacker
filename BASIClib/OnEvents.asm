; Intel 80386 ASM routines for ON EVENT GOTO support in BASIClib.
;
;    Copyright (c) 1998 Ryan C. Gordon and Gregory S. Read.

                              
public __callOnEventHandler

; This procedure copies the stack from event handling procedure to top
;  of stack, calls event handler, and copies stack back. Neet, huh?
;
;     params : POnEventHandler of handler to call at [esp + 4].
;    returns : any return value from BASIC routine in EAX.

__callOnEventHandler    proc    near
        push    ebp                     ; Save calling function's base ptr.
        mov     ebp,esp                 ; Move stack ptr to base ptr.

        push    ebx                     ; Save registers...

        mov     ebx,[ebp + 8]           ; Store POnEventHandler in EBX.
        mov     eax,[ebx]               ; Store handlerAddr in EAX.
        mov     edx,[ebx + 8]           ; Store stackEnd in EDX.
        mov     ebx,[ebx + 4]           ; Store stackStart in EBX.
        mov     ecx,ebx                 ;  ..save extra copy in ECX.

@stackcopy:                             ; loop to move stack...
        cmp     edx,ebx                 ; have we hit the end?
        ja      @endstackcopy           ; then stop.
        mov     byte ptr [esp],[ebx]    ; copy a byte.
        dec     esp                     ; point to next byte...
        dec     ebx                     ;   ...remember stack goes downward.
        jmp     @stackcopy              ; do it again.
@endstackcopy:

        mov     ebx,ebp                 ; Save copy of base pointer.
        mov     ebp,esp                 ; Fake the base ptr for handler.
        call    [eax]                   ; execute BASIC error handler.
                                        ;  retVal is in EAX.
        mov     ebp,ebx                 ; Move base pointer back.

@movestackback:                         ; Move it back.
        cmp     edx,ecx                 ; At start of stack (ECX)?
        jb      @endstackback           ;  then stop.
        mov     byte ptr [edx],[esp]    ; copy a byte.
        inc     edx                     ;  (inc?) decrement pointers.
        inc     esp
        jmp     @movestackback          ; do it again.
@endstackback:

        pop     ebx                     ; Restore registers...

        mov     esp,ebp                 ; Restore original stack pointer.
        pop     ebp                     ; Restore calling function's base ptr.
        ret                             ; Return.
__callOnEventHandler    endp

; end of OnEvents.asm ...


