.model tiny
.code
org 80h
length_cmd      db ?
line_cmd        db ?
org 100h

start:
    mov cl, [length_cmd]
    cmp cl, 1
    jle loop_param

    mov si, 0
    mov di, offset line_cmd

find_param:
    inc di
    mov ax, [di]
    mov ah, 0
    cmp al, ' '
    je next_param
    cmp al, 0Dh
    je too_low
    mov file_name[si], al
    inc si
    jmp find_param

too_low:
    4mov ah, 9
    mov dx, offset too_low_str
    int 21h
    ret

next_param:
    mov file_name[si], 0
    mov si, 0
loop_param:
    inc di
    mov ax, [di]
    mov ah, 0
    cmp al, 0Dh
    je end_param
    mov word_for_find[si], al
    inc si
    jmp loop_param

end_param:
    mov word_for_find[si], '$'

    mov dx, offset file_name_write
    mov ah, 5Bh
    mov cx, 0
    int 21h
    jnc ok
    
    mov dx, offset file_name_write
    mov ah, 3Dh
    mov al, 02h
    int 21h
ok:
    mov [file_descriptor], ax

    mov dx, offset file_name
    mov ah, 3Dh
    mov al, 00h
    int 21h
    jc cant_open
    mov bx, ax

read_buffer:
    mov cx, 1
    mov dx, offset word_buffer
    mov ah, 3Fh
    int 21h
    jc close_file
    mov cx, ax
    jcxz close_file_and_check
    call get_str
    jmp read_buffer
to_exit:
    jmp exit
close_file_and_check:
    mov si, [length_line]
    mov line_buffer[si], '$'
    call find_word
close_file:
    mov ah, 3Eh
    int 21h
    mov bx, [file_descriptor]
    mov ah, 3Eh
    int 21h
exit:
    ret

cant_open:
    mov ah, 9
    mov dx, offset cant_open_str
    int 21h
    ret

get_str proc
    push dx
    push di
    mov di, [length_line]
    mov dl, [word_buffer]
    mov line_buffer[di], dl
    add [length_line], cx
    pop ax
    pop dx

    cmp [word_buffer], 0Ah
    jne exit_get
    mov si, [length_line]
    mov line_buffer[si], '$'
    call find_word
    mov [length_line], 0
exit_get:
    ret
get_str endp 

find_word proc 
    push cx
    push dx

    lea di, line_buffer
    lea si, word_for_find 
    dec si
    dec di
loop_find:
    inc si
loop_find1:
    inc di
    mov dx, [si]
    mov dh, 00
    cmp dx, '$'
    je found
    mov cx, [di]
    mov ch, 00
    cmp cx, '$'
    je not_found
    cmp cx, ' '
    je next
    cmp cx, dx
    je loop_find
    
loop_space:
    lea si, word_for_find 
loop_space1:
    mov cx, [di]
    mov ch, 00
    cmp cx, ' '
    je loop_find1
    cmp cx, '$'
    je not_found
    inc di
    jmp loop_space1
not_found:
    ; mov ah, 9
    ; mov dx, offset line_buffer
    ; int 21h
    call write
    mov [length_line], 0
    jmp real
found:
    mov cx, [di]
    mov ch, 00
    cmp cx, 0Ah
    je real
    cmp cx, '$'
    je real
    cmp cx, ' '
    jne loop_space
real:
    pop dx
    pop cx
    ret
next:
    lea si, word_for_find 
    dec si
    jmp loop_find
endp

write proc 
    push ax
    push bx
    push cx
    push dx
    mov ah, 40h
    mov bx, [file_descriptor]
    mov cx, [length_line]
    mov dx, offset line_buffer
    int 21h
    pop dx
    pop cx
    pop bx
    pop ax
    ret
endp

file_name       db 1024 dup('?')
word_buffer     db ?
line_buffer     db 63000 dup('$')
length_line     dw 0
word_for_find   db "hello$"
file_name_write db "out.txt", 0
file_descriptor dw ?
too_low_str     db "Too low arguments!",0Ah,0Dh,'$'
cant_open_str   db "Cant open file!",0Ah,0Dh,'$'
calc            db 0

end start
