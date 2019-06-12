/*内存管理*/
/*
由系统调用，负责内存控制。
管理虚拟内存：
修改GDT表和页表
*/
#include"macro.h"
#include"_asm.h"

typedef unsigned int uint;
typedef unsigned short uint16;
typedef unsigned char byte;

//页目录，页表项结构
struct PAGEITEM{
	uint P:1;		//P：存在位；
	uint R_W:1;		//R/W：读写标志；
	uint U_S:1;		//U/S：用户特权级；
	uint nul1:2;	//空
	uint A:1;		//A：访问位
	uint D:1;		//D：脏位，由处理器设定，指示此页是否写入过数据
	uint nul2:2;	//空
	uint AVL:3;		//AVL：由软件设定
	uint PB:20;		//pagebase：页索引，此结构用作页目录时，指示每一项页表的位置；用作页表时，指示系统中每一页的位置
}__attribute__ ((aligned (1)));

typedef struct PAGEITEM pageitem;
//GDT表项结构
struct GDTABLE{
	uint16	limit0;
	uint16	base0;
	byte		base1;
	byte		TYPE:4;
	byte		S:1;
	byte		DPL:2;
	byte		P:1;
	byte		limit1:4;
	byte		AVL:1;
	byte		L:1;
	byte		D_B:1;
	byte		G:1;
	byte		base2;
}__attribute__ ((aligned (1)));

typedef struct GDTABLE GDTtable;

//地址映射函数，src：原地址，dst：被映射到的地址，size：长度(单位：页4kb)
void addressMap(unsigned int src,unsigned int dst,unsigned int size)
{
	//static pageitem *pl=(pageitem*)PAGE_LIST_ADDR;//页目录，1024项
	static pageitem *pt=(pageitem*)PAGE_TABLE_ADDR;//页表，1024*1024项
	pageitem pageTable={1,1,0,0,0,0,0,0,0};//页表项
	int i=0;
	for(i=0;i<size;++i){
		pageTable.PB=(src>>12)+i;
		pt[(dst>>12)+i]=pageTable;
	}
	return;
}
//设定指定GDT项
int set_gdt(int i,
		uint base,	//基地址
		uint limit,	//段限长，
		byte TYPE,	//段属性
		byte DPL,	//权限，0～3
		byte P,	//P=1，该段可使用
		byte AVL,	//由软件设定
		byte D_B,	//TYPE设定为代码段，D=1，表示32位程序；TYPE设定为数据段，B=1，最大访问范围4GB，表示为堆栈，B=1，使用esp
		byte G)	//G=0，段限长单位是字节，G=1，单位是4kb
{
	if(i==0)return GDT0_ERROR;//0号表项不可设定
	else if(i>=GDT_SIZE)return GDT_OUT_RANGE_ERROR;//最多不能超过2^13=8192项
	static GDTtable *gdt=(GDTtable*)GDT_ADDR;//GDT表的地址
	gdt[i].limit0=(limit&0xffff);
	gdt[i].limit1=((limit>>16)&0x0f);
	gdt[i].base0=(base&0xffff);
	gdt[i].base1=((base>>16)&0xff);
	gdt[i].base2=((base>>24)&0xff);
	gdt[i].TYPE=TYPE;
	gdt[i].S=1;
	gdt[i].DPL=DPL;
	gdt[i].P=P;
	gdt[i].AVL=AVL;
	gdt[i].L=0;
	gdt[i].D_B=D_B;
	gdt[i].G=G;
	return 0;
}

//初始化系统内存
/*对内存进行分配，将0～0xfff作为系统参数储存区，禁止权限大于0的用户修改*/
char initMemory()
{
	/*初始化页目录，页表*/
	
	//展开地址映射，但映射地址不变
	addressMap(0,0,1024*1024);
	//初始化GDT表
	set_gdt(1,0,0xfffff,GDT_RUN_R,0,1,0,1,1);//重设平坦模型下的代码段
	set_gdt(2,0,0xfffff,GDT_RW,0,1,0,1,1);//重设平坦模型下的数据段
	set_gdt(3,KERNEL_ADDR,STACK_SIZE,GDT_RW_DOWN,0,1,0,1,1);//堆栈放在内核前面
	set_gdt(4,0xb8000,1,GDT_RW,0,1,0,1,1);//显示区
	addressMap(0xb8000,0xc0000000,1);//将显示缓存区映射到高地址0xc0000000
	//初始化内存分配表，以物理地址为基础，储存已分配的地址和为分配的地址
	char* ch=(char*)0;
	return ch[0];
}

