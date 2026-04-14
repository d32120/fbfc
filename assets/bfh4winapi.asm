;CALLING CONVENTIONS WINAPI
;1 -> rcx
;2 -> rdx
;3 -> r8
;4 -> r9
;5,... -> on stack at index (ARG_COUNT-ARG_NUMB)*8
; calling conventions : https://www.reddit.com/r/asm/comments/1g0j2m6/how_to_use_win32_api_for_io_in_x86_assembly_masm/



; r10 -> current position (>,<)
; r11 -> memory start (0)
; rax, rcx, rdx -> used in subroutines
section .bss
    
    
section .data
    inputHandle    dq 0 ;it's a pointer so 1 quadword it's enough
    outputHandle   dq 0 ;it's a pointer so 1 quadword it's enough
    bytesRead      dq 0 ;unused (ignorable return value)
    bytesWritten   dq 0 ;unused (ignorable return value)
    arp  TIMES 100 db 0 ; for test
    
section .text
global main

extern GetStdHandle      ;import all
extern ReadConsoleA
extern WriteConsoleA
extern ExitProcess

         ; returns: handle rax
         ; uses: rcx, rax
    getOutputHandle:
         ; docs : https://learn.microsoft.com/en-us/windows/console/getstdhandle
         ; GetStdHandle(handleNo)
         mov rax,[outputHandle] ; load outputHandle
         test rax,rax           ; check if initialized     
         jnz gohret             ; if initialized go to return
         mov rcx, -11           ; STD_OUTPUT_HANDLE
         call GetStdHandle      ; get an handle (it will be rax)
         mov [outputHandle], rax;save the value
       gohret:
         ret
         
        ; returns: handle rax
        ; uses: rcx, rax
    getInputHandle:
        ; docs : https://learn.microsoft.com/en-us/windows/console/getstdhandle
                               ; GetStdHandle(handleNo)
        mov  rax,[inputHandle] ; load inputHandle
        test rax,rax           ; check if initialized     
        jnz  gihret            ; if initialized go to return
        mov  rcx, -10          ; STD_INPUT_HANDLE
        call GetStdHandle      ; get an handle (it will be rax)
        mov  [inputHandle], rax; save the value
      gihret:
        ret
    
    
        ; input: r10 (current position)
        ; sets the current position to the char read
    read_char:
        call getInputHandle        ; get the handle for input: now rax = handle 
       ;sub  rsp, 32               ; we should do that because is needed for some reason (WinApi wants 32 bytes alignment) -|
        ; calling ReadConsoleA(handle, &dest, 1, &bytesRead, NULL)                                                          |
        mov  rcx, rax              ; handle       (1->rcx)                                                                  |
        lea  rdx, [r10]            ; &dest        (2->rdx)                                                                  |
        mov  r8, 1                 ; 1 (1 char)   (3->r8)                                                                   |
        lea  r9, [rel bytesRead]   ; unused       (4->r9)                                                                   |
        push qword 0               ; NULL         (5-> on stack)                   but instead we do that and then sub 24 <-|
        sub  rsp, 24               ; to keep 32 bytes alignment
       
        call ReadConsoleA          ; also called ReadConsole : https://learn.microsoft.com/it-it/windows/console/readconsole

        add rsp, 32                ; stack cleanup
        ret
        
        
        ; input: r10 (current position)
        ; prints the current position as an ASCII char
        ; leave unchanged the current position
    print_char:
        call getOutputHandle      ; get the handle for output: now rax = handle 
        ;sub rsp, 32              ;see before to understand why 
        ; calling WriteConsoleA(handle, &src, 1, &bytesWritten, NULL)
        mov  rcx, rax             ; handle
        lea  rdx, [r10]           ; &src (will be the current position)
        mov  r8, 1                ; 1 (1 char)
        lea  r9,[rel bytesWritten]; unused (feeeeeew it barely fits with indentation)
        push qword 0              ; NULL (it must be null idk why)
        sub  rsp, 24              ;see before
        call WriteConsoleA        ; also called WriteConsole : https://learn.microsoft.com/it-it/windows/console/writeconsole

        add rsp, 32               ; stack cleanup
        ret

     EXIT:
        xor  rcx,rcx
        call ExitProcess
    main:
        xor  rbx,rbx
        add [arp+rbx], 30h       ;should print '0'
        call print_char
        call EXIT                ; exits