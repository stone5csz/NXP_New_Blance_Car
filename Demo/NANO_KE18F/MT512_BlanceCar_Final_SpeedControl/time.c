#include "time.h"

//精确延时函数wait
void wait(unsigned long n)
{
	do{
      n--;
    }while(n);
}
