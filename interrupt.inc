;name:interrupt.inc
;coding:UTF-8
;说明：本文包含0～0x2f的中断程序

;int 0================================================================================================
div_error:		;除法指令出错,返回时会回到该指令，因此要通过堆栈读取异常代码的位置，令寄存器ebx，edx归零，然后跳过
	mov ebx,0
	mov edx,0
	push eax
	mov eax,[esp+4]
	add eax,2
	mov [esp+4],eax
	pop eax
	iret

;int 1=================================================================================================
debug_error:		;调试异常
	iret

;int 2=================================================================================================
_not_2:			;没有指令
	iret

;int 3=================================================================================================
call_int_3:			;使用了int 3
	iret

;int 4=================================================================================================
overflow:			;发生溢出
	iret

;int 5=================================================================================================
boundary_check:		;边界检查
	iret

;int 6=================================================================================================
illegal_instruction:	;非法指令
	iret

;int 7=================================================================================================
unavailable_device:	;设备不可用
	iret

;int 8=================================================================================================
double_error:		;双重故障
	iret

;int 9=================================================================================================
coprocessor_seg_error:	;协处理器段越界
	iret

;int 10=================================================================================================
invalid_TSS:		;无效TSS
	iret

;int 11=================================================================================================
no_seg:			;段不存在
	iret

;int 12=================================================================================================
stack_seg_error:		;堆栈段异常
	iret

;int 13=================================================================================================
protect_error:		;通用保护异常
	iret

;int 14=================================================================================================
page_error:			;页异常
	iret

;int 15=================================================================================================
_not_15:			;没有15号中断
	iret

;int 16=================================================================================================
coprocessor_error:	;协处理器出错
	iret


;int 0x20=================================================================================================
timer_int:			;计时器中断
	mov ax,0x10
	mov ebx,str0x20
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
;timer_int:
	iret

;int 0x21=================================================================================================
key_int:
pusha
	in al,0x60
;判断是否是数字
	cmp al,0xb
	jle @f
	jmp code_to_ascii
@@:
	cmp al,0x0b
	je @f
	
	add al,0x2f
	jmp put_char
@@:
	mov al,0x30
	jmp put_char
;判断文本
code_to_ascii:
	macro trans_scan scan_code,ascii_code
		{
				cmp al,scan_code
				jne @f
				mov al,ascii_code
				jmp put_char
			@@:
		}
	trans_scan 0x0e,0x08
	trans_scan 0x0f,0x09
	trans_scan 0x10,0x71
	trans_scan 0x11,0x77
	trans_scan 0x12,0x65
	trans_scan 0x13,0x72
	trans_scan 0x14,0x74
	trans_scan 0x15,0x7a
	trans_scan 0x16,0x75
	trans_scan 0x17,0x69
	trans_scan 0x18,0x6f
	trans_scan 0x19,0x70
	trans_scan 0x1a,0x5b
	trans_scan 0x1b,0x5d
	trans_scan 0x1c,0x0d
	trans_scan 0x1e,0x61
	trans_scan 0x1f,0x73
	trans_scan 0x20,0x64
	trans_scan 0x21,0x66
	trans_scan 0x22,0x67
	trans_scan 0x23,0x68
	trans_scan 0x24,0x6a
	trans_scan 0x25,0x6b
	trans_scan 0x26,0x6c
	trans_scan 0x27,0x3b
	trans_scan 0x28,0x27
	trans_scan 0x29,0x60
	trans_scan 0x2b,0x5c
	trans_scan 0x2c,0x79
	trans_scan 0x2d,0x78
	trans_scan 0x2e,0x63
	trans_scan 0x2f,0x76
	trans_scan 0x30,0x62
	trans_scan 0x31,0x6e
	trans_scan 0x32,0x6d
	trans_scan 0x33,0x2c
	trans_scan 0x34,0x2e
	trans_scan 0x35,0x2f
	trans_scan 0x39,0x20
cmp al,0x35
jle put_char

;打印al中的字符
put_char:
	mov ah,2
	call print_char
	mov al,0x20
  	out 0x20,al
end_key:
	
popa
iret
;转换函数，将键盘扫描码转换为ascii字符



;int 0x22=================================================================================================
_irq9:				;与irq9相连
	mov ax,0x10
	mov ebx,str0x22
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x23=================================================================================================
com2_int:
	mov ax,0x10
	mov ebx,str0x23
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x24=================================================================================================
com1_int:
	mov ax,0x10
	mov ebx,str0x24
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x25=================================================================================================
lpt2_int:			;声卡
	mov ax,0x10
	mov ebx,str0x25
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x26=================================================================================================
fdd_int:			;软驱控制器
	mov ax,0x10
	mov ebx,str0x26
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x27=================================================================================================
lpt1_int:			;打印机传输控制
	mov ax,0x10
	mov ebx,str0x27
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x28=================================================================================================
cmos_alert_int:		;即时时钟
	mov ax,0x10
	mov ebx,str0x28
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x29=================================================================================================
_irq2:			;与irq2相连
	mov ax,0x10
	mov ebx,str0x29
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x2a=================================================================================================
reversed1_int:		;建议给网卡
	mov ax,0x10
	mov ebx,str0x2a
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x2b=================================================================================================
reversed2_int:		;建议给APG显卡
	mov ax,0x10
	mov ebx,str0x2b
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x2c=================================================================================================
ps_2_mouse_int:		;鼠标
	mov ax,0x10
	mov ebx,str0x2c
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x2d=================================================================================================
fpu_int:			;协处理器
	mov ax,0x10
	mov ebx,str0x2d
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x2e=================================================================================================
primary_ide_int:		;主硬盘传输控制
	mov ax,0x10
	mov ebx,str0x2e
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	iret

;int 0x2f=================================================================================================
seccondary_ide_int:	;从硬盘传输控制
	push eax
	push ebx
	push ecx
	push edx
	mov ax,0x10
	mov ebx,str0x2f
	mov ecx,8
	mov dl,0x02
	call print_string
	mov al,0x20
  	out 0x20,al
	pop edx
	pop ecx
	pop ebx
	pop eax
	iret
;数据==========================================================================

str0x20 db "int 0x20"
str0x21 db "int 0x21"
str0x22 db "int 0x22"
str0x23 db "int 0x23"
str0x24 db "int 0x24"
str0x25 db "int 0x25"
str0x26 db "int 0x26"
str0x27 db "int 0x27"
str0x28 db "int 0x28"
str0x29 db "int 0x29"
str0x2a db "int 0x2a"
str0x2b db "int 0x2b"
str0x2c db "int 0x2c"
str0x2d db "int 0x2d"
str0x2e db "int 0x2e"
str0x2f db "int 0x2f"







































