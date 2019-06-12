;name:boot.asm
;coding:UTF-8

include 'macro.inc'	;包含参数的宏

__head equ 0x7e00
use16
org 0x7c00
_start:
	xor ax,ax
	mov es,ax
	mov ds,ax

;设定为字符模式，80*25
	mov ax,0x03
	int 0x10
;记录系统开机时间，读取cmos,存放在0xff0,占用8字节：$|秒|分|时|日|月|年|$
	mov byte [START_CMOS_TIME],'$'
	mov al,0
	out 0x70,al
	in al,0x71		;秒
	mov [START_CMOS_TIME+1],al

	mov al,2
	out 0x70,al
	in al,0x71		;分
	mov [START_CMOS_TIME+2],al

	mov al,4		;时
	out 0x70,al
	in al,0x71
	mov [START_CMOS_TIME+3],al

	mov al,7		;日
	out 0x70,al
	in al,0x71
	mov [START_CMOS_TIME+4],al
	
	mov al,8		;月
	out 0x70,al
	in al,0x71
	mov [START_CMOS_TIME+5],al

	mov al,9		;年
	out 0x70,al
	in al,0x71
	mov [START_CMOS_TIME+6],al
	mov byte [START_CMOS_TIME+7],'$'

;文件系统代号,将代号传送到0xff8，8字节
	mov si,filesys
	mov di,FILE_SYSTEM
	mov cx,2
	rep movsd

;使用bios加载head
	mov ax,0x0200+HEAD_SIZE	;ah=2,读扇区；al=15,读取扇区数
	mov cx,0x0002		;ch=0,0柱面；cl=1,1号扇区
	mov dx,0x0080		;dh=0,0磁头；dl=0x80,硬盘
	mov bx,__head		;es:bx=0:0x7e00,被加载的地址
	int 0x13

;设定GDT表
	cli
	mov si,gdt_start
	mov di,GDT_ADDR
	mov cx,10
	rep movsd	;将GDT复制到GDT_TABLE位置
	;准备进入保护模式
	lgdt ptr gdt_size
;打开A20地址线
	in al,0x92
	or al,0000_0010b
	out 0x92,al

;设置PE位，进入保护模式
	mov eax,cr0
	or eax,1
	mov cr0,eax
;清空流水线，并串行化处理器
	jmp dword 0x08:__head

spin:
	hlt
	jmp spin
	
filesys db "lx1.0$$$"
;==================================================================
align 2
GDT_SIZE equ 5
gdt_size:
	dw GDT_SIZE*8-1
	dd GDT_ADDR

align 4
gdt_start:
	dd 0,0
;#1，平坦模型，代码段，段选择子0x8
;段基址，0；段界限0xfffff；描述符子类型（TYPE）=1010b，代码段=执行、可读;描述符类型（S）=1，代码段；特权级（DPL）=0；段存在位（P）=1；
;软件位（AVL）=0；64位段标志（L）=0；D位=1；G位=1，以4kb为单位；
	GDTitem 0,0xfffff,1010b,1,0,1,0,0,1,1
;#2，平坦模型，数据段，段选择子0x10
	GDTitem 0,0xfffff,0010b,1,0,1,0,0,1,1
;#3，栈段，设定为内核堆栈，段选择子0x18
	GDTitem KERNEL_ADDR,STACK_SIZE,0110b,1,0,1,0,0,1,1
;#4，显示缓冲区，段选择子0x20
	GDTitem 0xb8000,1,0010b,1,0,1,0,0,1,1

times 254-($-$$) db 0
;分区a


times 510-($-$$) db 0
dw 0xaa55

