;
; G -- Real-mode image loader
; 
; Copyright (C) 2010  Anonymous
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to 
; the Free Software Foundation, Inc.
; 51 Franklin Street, Fifth Floor
; Boston, MA  02110-1301, USA
;

%define imgaddr 0x500
%define v_mode 0x112
%undef load_palette

org 0x7C00

start:
cli
mov ax, 0x9000
mov ss, ax
xor sp, sp

xor ax, ax
mov ds, ax 

mov al, [cur_drive]
cmp al, 0x00
jnz reset_drive
mov [cur_drive], dl

reset_drive:
xor ax, ax
xor dx, dx
mov dl, [cur_drive]
int 0x13
jc reset_drive

mov ah, 0x41
mov bx, 0x55AA
int 0x13
jc chs
cmp bx, 0XAA55
jnz chs
and cx, 1
jnz read_ready

chs:
mov bx, 0x3F0
mov dx, [lba_addr2]
mov ax, [lba_addr1]
div bx
mov [cur_cylinder], ax
mov ax, dx
mov bl, 63
div bl
mov [cur_head], al
inc ah
mov [cur_sector], ah
chs_probe_drive:
mov dl, [cur_drive]
mov ah, 0x08
int 0x13
jc chs_probe_drive
mov [max_head], dh
mov dh, cl
shr dh, 6
mov dl, ch
mov [max_cylinder], dx
and cl, 0x3F
mov [max_sector], cl
cmp dx, 0x000
jz chs_read
mov ax, 0x03ff
mov [max_cylinder], ax

read_ready:
mov ax, 0x7E0
call do_read
jmp gfx

do_read:
mov bl, [max_sector]
cmp bl, 0
jnz use_chs
call lba_read
retn
use_chs:
call chs_read
retn

chs_read:
mov es, ax
xor bx, bx
mov ax, [cur_cylinder]
mov ch, al
mov cl, ah
shl cl, 6
mov ah, [cur_sector]
or cl, ah
mov dh, [cur_head]
mov dl, [cur_drive]
mov ax, 0x0201
int 0x13
jc chs_read
mov al, [cur_sector]
inc al
mov bl, [max_sector]
cmp al, bl
jle chs_sec_wb
mov al, [cur_head]
inc al
mov bl, [max_head]
cmp al, bl
jle chs_hea_wb
mov ax, [cur_cylinder]
inc ax
mov [cur_cylinder], ax
xor ax, ax
chs_hea_wb:
mov [cur_head], al
mov al, 1
chs_sec_wb:
mov [cur_sector], al
retn

lba_read:
mov [lba_buf_seg], ax
mov [lba_blockcnt], word 0x0001
mov dl, [cur_drive]
mov ah, 0x42
mov si, lba_pktsiz
int 0x13
jc lba_read
mov ax, [lba_addr1]
inc ax
mov [lba_addr1], ax
jno lba_read_done
mov ax, [lba_addr2]
inc ax
mov [lba_addr2], ax
jno lba_read_done
mov ax, [lba_addr3]
inc ax
mov [lba_addr3], ax
jno lba_read_done
mov ax, [lba_addr4]
inc ax
mov [lba_addr4], ax
lba_read_done:
retn

lba_pktsiz      db      0x10
lba_reserved    db      0x0
lba_blockcnt    dw      0x01
lba_buf_addr    dw      0x0
lba_buf_seg     dw      0x0
lba_addr1       dw      0x0001
lba_addr2       dw      0x0000
lba_addr3       dw      0x0000
lba_addr4       dw      0x0000

cur_drive       db      0x0
cur_cylinder    dw      0x0
cur_head        dw      0x0
cur_sector      db      0x0
max_cylinder    dw      0x0
max_head        db      0x0
max_sector      db      0x0

section .signatur start=0x7DFE
sig db 0x55, 0xAA

;******************************************************************************

section .stage2 start=0x7E00

gfx:
mov cx, v_mode
mov ax, imgaddr 
mov es, ax
mov di, 0x00
mov ax, 0x4F01
int 0x10
mov ax, [es:0x04] 
shl ax, 10 
jnc probe_video2
mov ax, 0xffff
probe_video2:
mov [sv_offset_max], ax
mov ax, [es:0x1A]
mov [sv_page_max], ax
mov ax, [es:0x08]
cmp ax, 0x0000
jz probe_video_done
mov [sv_seg], ax
probe_video_done:

mov ax, 0x4F02
mov bx, v_mode
int 0x10

load_gfx:
mov ax, imgaddr
call do_read
call putmpx
cmp ax, 0
jz load_gfx

ende:
jmp ende

putmpx:
xor bx, bx
%ifdef load_palette
; TODO: Also try palette BIOS call
mov dx, [palette_count]
cmp dx, 256
jz putmpx_do
mov ax, imgaddr
mov es, ax
xor bx, bx
xor cx, cx
pattern_loop:
mov dx, 0x03C8
mov ax, [palette_count]
add ax, cx
out dx, al
inc dx
mov al, [es:bx+2]
out dx, al
inc bx
mov al, [es:bx]
out dx, al
inc bx
mov al, [es:bx-2]
out dx, al
inc bx
;mov al, [es:bx]
;out dx, al
inc bx
inc cx
cmp cl, 128
jnz pattern_loop

mov dx, [palette_count]
add dx, cx 
mov [palette_count], dx
xor ax, ax
retn
%endif
putmpx_do:
mov ax, imgaddr
mov es, ax
mov al, [es:bx]
push bx
push ax
xor bx, bx
mov cx, [sv_offset]
mov dx, [sv_page]
mov ax, [sv_seg]
mov es, ax
cmp cx, 0
jnz putpx_start
mov ax, 0x4F05
int 0x10
putpx_start:
mov si, cx
pop ax
mov [es:si], al
xor ax, ax
mov bx, [sv_offset_max]
cmp cx, bx
jnz putpx_done
xor cx, cx
inc dx
mov bx, [sv_page_max]
cmp dx, bx
jnz putpx_done_ni
xor dx, dx
mov ax, 1
jmp putpx_done_ni
putpx_done:
inc cx
putpx_done_ni:
mov [sv_offset], cx
mov [sv_page], dx
pop bx
cmp ax, 0 
jnz putmpx_done
inc bx
cmp bx, 512
jnz putmpx_do
putmpx_done:
retn

palette_count	dw	0x0
sv_seg          dw      0xA000
sv_offset       dw      0x0
sv_page         dw      0x0
sv_offset_max   dw      0x0
sv_page_max     dw      0x0

section .endblock start=0x7FFF
endb db 0x0
