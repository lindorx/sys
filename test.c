#include"memory.h"
#include"sysio.h"

int main()
{
initMemory();
char str[11]="1234567890\0";
sysprintf(2,str);
return 0;
}
