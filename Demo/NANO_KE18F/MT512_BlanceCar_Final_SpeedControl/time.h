void wait(unsigned long n);

//精确延时函数
#define  SYSCLK          150         //指明CPU工作频率为150MHz
#define  A               6           //一次循环所花的周期数
#define  B               3           //调用、初始化、返回总共所用的周期数
#define  delay_us(nus)   wait(((nus)*(SYSCLK)-(B))/(A))
#define  delay_ms(nms)   delay_us((nms)*1000)
#define  delay_s(ns)     delay_ms((ns)*1000)
//使用方法：
//delay_us(0.4);   //延时0.4us
//delay_ms(1456);  //延时1.456s
//delay_s(21.4345);//延时21.4345秒
