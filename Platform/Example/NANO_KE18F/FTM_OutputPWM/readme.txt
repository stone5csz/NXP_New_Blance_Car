
在运行测试程序时：
1. 先将示波器的采样端连接到PTD15(FTM0_CH0)引脚上。
2. 可执行“pin_testchannel”命令，以GPIO输出的方式翻转PTD15引脚的输出电平，若是在示波器上观察到信号的变化，说明连接无误。
3. 执行“ftm_config_pwm 10000”命令，初始化FTM0模块以PWM方式工作。其中输入参数10000为FTM定时器计数模值。
4. 执行“ftm_output_pwm 5000”命令，配置FTM0通道0的翻转点为5000。此时观察示波器输出应为一个占空比为50%的波形，频率为934.6Hz。见图“%50.jpg”
5. 再执行“ftm_output_pwm 1000”命令，观察示波器输出波形的频率不变，但是占空比变约为10%，高电平的时间长度较短，低电平的时间长度较长，见图“%10.jpg”。这就说明“eFTM_ChannelMode_PWM_HighTruePulse”确实初始输出为高电平，在翻转点变为了低电平。


根据输出波形计算System Clock的频率。
934.6Hz x 10000(计数模值) x 16 (FTM计数分频) = 149536000
约等于150MHz，同预期匹配。