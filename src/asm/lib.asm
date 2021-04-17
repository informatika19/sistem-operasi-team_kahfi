; lib.asm
; Alvin W., Josep M., Rehagana K.C.S.
; 20 Februari 2021

; lib.asm berisi fungsi-fungsi bantuan kernel yang tidak bisa didefinisikan di
; file lib.c

global _getCursorRow
global _getCursorCol


;int getCursorCol()
_getCursorRow:
	push bp
	mov bp,sp
	mov ah,0x03
	xor bh,bh
	int 0x10

	mov al,dh	;ambil row nya
	mov ah,0	;return row yg ditaro di AL
	pop bp
	ret

;int getCursorCol()
_getCursorCol:
	push bp
	mov bp,sp
	mov ah,0x03
	xor bh,bh
	int 0x10

	mov al,dl	;ambil col nya // al = dl
	mov ah,0	;return col yg ditaro di AL
	pop bp
	ret