void wait(unsigned long n);

//��ȷ��ʱ����
#define  SYSCLK          150         //ָ��CPU����Ƶ��Ϊ150MHz
#define  A               6           //һ��ѭ��������������
#define  B               3           //���á���ʼ���������ܹ����õ�������
#define  delay_us(nus)   wait(((nus)*(SYSCLK)-(B))/(A))
#define  delay_ms(nms)   delay_us((nms)*1000)
#define  delay_s(ns)     delay_ms((ns)*1000)
//ʹ�÷�����
//delay_us(0.4);   //��ʱ0.4us
//delay_ms(1456);  //��ʱ1.456s
//delay_s(21.4345);//��ʱ21.4345��
