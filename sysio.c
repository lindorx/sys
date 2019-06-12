#include"macro.h"
#include"_asm.h"
//从端口读取字符串位置
int get_char_pos()
{
	//向0x3d4端口写入0x0e，从0x3d5端口读取到高8位
	//向0x3d4端口写入0x0f，从0x3d5端口读取到低8位
	uint8 a=0x0e;
	uint8 b=0;
	asm_outb(&a,0x3d4);
	asm_inb(&b,0x3d5);
	a=0x0f;
	asm_outb(&a,0x3d4);
	asm_inb(&a,0x3d5);
	return (int)((int)b<<8)+a;
}

int sysprintf(int size,char * str)
{
	static char* dp=(char*)TEXT_DISPLAY_MODE;//显示缓冲区
	int temp=get_char_pos();//字符指针，时刻指向最后一个字符串的位置
	int i;
	for(i=0;i<size;++i){
		dp[temp+i]=str[i];
	}
	return i;
}
