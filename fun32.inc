;name:fun32.inc
;coding:UTF-8
;说明：本文件包含bootloder使用的32位函数
align 8
use32
;print_char==>>向屏幕打印一个字符；参数：al=ascii字符，ah=前背景色。不修改任何寄存器

;print_string==>>向屏幕打印字符串；参数：ax=字符串所在段的段选择子，ebx=偏移量，ecx=字符数，dl=前背景色。不修改任何寄存器

;put_pos==>>修改光标位置；参数：ecx=光标所在位置。不修改任何寄存器

;get_pos==>>获取光标位置；参数：无。修改ecx寄存器

;scan_key_get==>>从键盘获取扫描码；参数：无。修改eax寄存器，若eax为0，说明未能读取到扫描码，建议重复尝试

;key_get==>>将键盘扫描码转换为ascii码，部分键盘不进行转换；参数：无。修改eax、ecx寄存器，
;al为转换后的字符，如果扫描码超过一位，则ah存入0xe0，al存入另一位扫描码。ch=0，键盘弹起；ch=1，键盘按下；
;cl=0，扫描码为两字节；cl=1，可显示字符;cl=2，为转换字符。

;print_hex==>>将ebx寄存器的值以16进制打印出来。参数：ebx=需要打印的值。不修改任何寄存器的值。

;read_disk==>>磁盘读取（仅SATA硬盘，LBA28），ebx=读取扇区号，cl=读取扇区数，es=数据段选择子，edi=被加载的位置

;宏
KEYGET_MAX_TRY_TIMES EQU 100

;向屏幕打印一个字符32位*******************************************************
;要显示的字符存入al，前背景色存入ah
print_char:
	pushad
	push ds
	mov cx,0x20		;设定段选择子，选择是gdt的#4表项定义的数据段,这里指向画面缓冲区
	mov es,cx
	mov ds,cx
	call get_pos		;获得光标的位置,存在ecx中
	imul ebx,ecx,2		;ebx为当前字符区偏移量
judge:		;判断字符类型
judge_back:	;退格
	cmp al,0x08
	jz ch_back
	jmp judge_tab
ch_back:
	cmp ecx,0
	jz end_char
	sub ebx,2
	mov word [es:ebx],0x0700	;清除字符
	dec ecx				;令光标后退一格
	call put_pos
	jmp end_char
judge_tab:
	cmp al,0x09
	jz ch_tab
	jmp judge_enter
ch_tab:
	mov esi,2000
	mov eax,ecx
	and eax,111b
	cmp eax,0
	jz a1
	jmp a2
a1:
	add eax,8
a2:
	mov edi,2000
	add edi,eax
	imul edi,2
	imul esi,2
	;设置转移次数
	mov eax,2000
	sub eax,ecx
	shr eax,1
	push ecx
	mov ecx,eax
	inc ecx
	std
	rep movsd
	cld
	mov ecx,4
loop1:
	mov dword [es:ebx],0x07200720
	add ebx,4
	loop loop1
	pop ecx
	add ecx,8
	call put_pos
	jmp end_char

judge_enter:
	cmp al,0x0d
	jz ch_enter
	jmp judge_newline
ch_enter:
	mov ax,cx
	mov bl,80
	div bl		;商在al，余数在ah
	movzx eax,ah
	sub cx,ax
	call put_pos
	jmp end_char

judge_newline:		;将光标平移到下一行
	cmp al,0x0a
	jz ch_newline
	jmp judeg_ch
ch_newline:
	cmp ecx,1840
	jae b1		;如果光标的位置大于等于1840，就将整体内容向前平移160字节
	jmp b2
b1:
	mov esi,160
	mov edi,0
	mov ecx,960
	rep movsd
	jmp end_char
b2:
	add ecx,80
	call put_pos
	jmp end_char
judeg_ch:	;判断字符是否可输出
	cmp al,0x20
	jl end_char
	cmp al,0x7e
	ja end_char
char:	;字符输出
	inc ecx
	call put_pos
	mov word [es:ebx],ax
end_char:
	pop ds
	popad
	ret
;--------------------------------------------------------------------------
;向屏幕打印一个字符串********************************************************
;使用前要先制定一个段选择子,指向要读取的数据段，存入ax。偏移量存入ebx,字符数存入ecx，前背景色存入dl
print_string:
pushad
push ds
mov ds,ax
mov ah,dl
loop_ch:
mov al,byte [ds:ebx]
call print_char
inc ebx
loop loop_ch
pop ds
popad
ret
;*************************************************************************

;字符显示程序32位下的*********************************************
;读取光标位置，返回到ecx中
get_pos:
push eax
push edx
xor ecx,ecx
mov ax,0x0f0e
mov dx,0x03d4
out dx,al
mov dx,0x03d5
in al,dx
mov ch,al

mov al,ah
mov dx,0x03d4
out dx,al
mov dx,0x03d5
in al,dx
mov cl,al
pop edx
pop eax
ret
;*******************************************************
;修改光标的位置，值存入cx
put_pos:
push eax
push edx
mov ax,0x0f0e
mov dx,0x03d4
out dx,al
mov dx,0x03d5
mov al,ch
out dx,al

mov al,ah
mov dx,0x03d4
out dx,al
mov dx,0x03d5
mov al,cl
out dx,al
pop edx
pop eax
ret
;*******************************************************
;键盘获得扫描码，返回到eax
scan_key_get:
	push ecx
	xor eax,eax
	mov ecx,KEYGET_MAX_TRY_TIMES		;尝试KEYGET_MAX_TRY_TIMES次
@@:
;数据端口0x60，命令端口0x64
;读取状态寄存器
	in al,0x64		;读取状态寄存器
	and al,0x01
	cmp al,0		;判断输出缓冲区
	jne next
	loop @b
	jmp @f
next:
	in al,0x60		;从数据端口读取信息
	times 10 nop
	;转换从端口获得的扫描码,al存入扫描码，返回al（和ah），由ebx说明转换码类型，1可显示字符，2控制字符
@@:
pop ecx
ret
;*******************************************************
;字符转换函数，讲寄存器储存的值转换为可显示的16进制字符串并显示出来
;待转换值存入ebx,前背景色存入ah
print_hex:
	push ecx
	push eax
	mov ecx,8
loop_num_hex:
	rol ebx,4
	mov al,bl
	and al,0x0f
	cmp al,9
	jg english_ch
	add al,0x30
	call print_char
	jmp @f
english_ch:
	add al,0x37
	call print_char
@@:
	loop loop_num_hex
	mov al,0x0d
	call print_char
	mov al,0x0a
	call print_char
	pop eax
	pop ecx
	ret
;*******************************************************
;键盘扫描码转换函数，用来将从键盘获取的扫描码转换为ascii字符，非可显示字符不进行转换
;返回ax。
;由ecx说明转换码类型，cl存入类型：1可显示字符，2未转换字符，0说明为超过一个字节的扫描码，即以0x0e开头的扫描码。(当ch=0时，无用)
;ch=1，键盘按下；ch=0，键盘弹起，弹起键不作转换处理。
key_get:
	;call scan_key_get
	xor ecx,ecx		;读取扫描码
	in al,0x60
fixed:
	mov ah,2
	cmp al,0xe0
	je tow_code
	cmp al,0x80		;当al的值大于等于（不等于0xe0）0x80时，说明键盘弹起
	jge key_down
	jmp @f
key_down:
	mov cx,0
	ret
@@:
	mov cx,0x0102
	;判断是否为数字
	cmp al,0x2
	jge @f
	jmp not_number
@@:
	cmp al,0x0b		;判断是否为数字键
	jle @f
	jmp not_number
@@:
	cmp al,0x0b		;判断是否为0
	je @f
	add al,0x2f
	jmp ch_is_ascii
@@:
	mov al,0x30
ch_is_ascii:
	mov ecx,1
	jmp end_num_to_ascii
;判断是否为其他字符
;创建一个宏,使用宏创建代码
not_number:
macro trans_scan scan_code,ascii_code
{
		cmp al,scan_code
		jne @f
		mov al,ascii_code
		jmp end_num_to_ascii
	@@:
}

trans_scan 0x0e,0x08
trans_scan 0x0f,0x09
trans_scan 0x10,0x71
trans_scan 0x11,0x77
trans_scan 0x12,0x65
trans_scan 0x13,0x72
trans_scan 0x14,0x74
trans_scan 0x15,0x79
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
trans_scan 0x2c,0x7a
trans_scan 0x2d,0x78
trans_scan 0x2e,0x63
trans_scan 0x2f,0x76
trans_scan 0x30,0x62
trans_scan 0x31,0x6e
trans_scan 0x32,0x6d
trans_scan 0x33,0x2c
trans_scan 0x34,0x2e
trans_scan 0x35,0x2f

;左ctrl 0x1d
;左shift 0x2a
;当扫描码多于1个的话，即第一个码为0xe0，在这里处理
jmp end_key_get
tow_code:
	;重新获取一个扫描码
	call scan_key_get
	mov ah,0xe0
	mov cl,0
	ret
end_num_to_ascii:
	mov cl,1
end_key_get:
	ret
;*******************************************************
;磁盘读取（仅SATA硬盘，LBA28），ebx=读取扇区号，cl=读取扇区数，es=数据段选择子，edi=被加载的位置
read_disk:
	pushad
	;检测磁盘状态，读取0x1f7端口，bit1用于控制磁盘运行，bit1=0，关闭磁盘；bit1=0，启动磁盘
	mov dx,0x1f7
	in al,dx
	or al,00000010b			;置0x1f7端口bit1为1，确保磁盘运行中		
	out dx,al
	;写读取扇区数
	mov dx,0x1f2
	mov al,cl
	out dx,al
	;写起始扇区，从高到低依次存放0x1f6(前四位)，0x1f5，0x1f4，x1f3
	inc dx		;0x1f3
	mov al,bl
	out dx,al

	inc dx		;0x1f4
	mov al,bh
	out dx,al

	inc dx		;0x1f5
	shr ebx,16
	mov al,bl
	out dx,al

	inc dx		;0x1f6
	and bh,0x0f
	mov al,bh
	or al,0xe0
	out dx,al

	inc dx		;0x1f7
	mov al,0x20
	out dx,al			;向0x1f7写入0x20开始读取磁盘

	;检查0x1f7端口的bit7是否为0且bit3是否为1，是则说明磁盘读取完成
@@:
	in al,dx
	and al,0x88
	cmp al,0x08
	jnz @b

	mov dx,0x1f0
	and ecx,0x000000ff
	imul ecx,256
@@:
	in ax,dx
	mov word [es:edi],ax
	add edi,2
	loop @b
	
	popad
	ret
;*******************************************************
