#ifndef MACRO_H
#define MACRO_H
#define START_CMOS_TIME 0xff0
#define FILE_SYSTEM 0xff8
#define CPUID_TXT 0
#define START_TIME_YEAR 2019
#define GDT_ADDR 0x1000

//内核定义
#define KERNEL_ADDR 0X600000
#define STACK_SIZE 0X1000

//页表定义
#define PAGE_LIST_ADDR 0x100000
#define PAGE_TABLE_ADDR 0x101000

typedef unsigned char byte;
typedef unsigned char uint8;    //8位无符号整数
typedef unsigned short uint16;   //16位无符号整数
typedef unsigned int uint32;     //32位无符号整数
typedef unsigned long uint64;    //64位无符号整数

typedef uint32 uint;
typedef int ERROR;			//错误类型


//GDT表代码定义
#define GDT_SIZE 8192		//gdt表项数量，一共2^13=8192项,0项不可设定，可用8191项
#define GDT0_ERROR -1		//尝试设定第0项gdt错误
#define GDT_OUT_RANGE_ERROR -2		//gdt超出界线
#define GDT_SET_ERROR -3		//向内存写入段选择子错误
	//段属性
	#define GDT_R 0			//0000 段只读
	#define GDT_RW 2			//0010 段可读可写
	#define GDT_R_DOWN 4		//0100 只读，向下扩展
	#define GDT_RW_DOWN 6		//0110 读写，向下扩展
	#define GDT_RUN 8			//1000 只执行
	#define GDT_RUN_R 0XA		//1010 执行，可读
	#define GDT_RUN_SAME 0xC		//1100 执行，一致
	#define GDT_RUN_R_SAME 0xE	//1110 执行，可读，一致

//地址映射
#define TEXT_DISPLAY_MODE 0XC0000000	//显示缓冲区地址
#endif
