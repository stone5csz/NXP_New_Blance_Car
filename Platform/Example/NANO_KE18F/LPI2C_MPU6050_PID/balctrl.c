/* balctrl.c */
#include "app_inc.h"
#include "math.h"

#define BALANCE_ANGLE   0

/**************************************************************************
* function: simple Kalman filter
* parameters: none
* returns: result includes angle and palstance
**************************************************************************/

void Kalman_Filter(const MPU6050_Info_T *info, Pos_Angle_Info_T *pos)      
{
    //static float K1 = 0.02;
    static double angle_dot = 0;
    static double angle = 0;
    static float Q_gyro = 0.003;
    static float Q_angle = 0.001;   // 过程噪声的协方差
    static float R_angle = 0.5;     // 测量噪声的协方差 既测量偏差
    static float dt = 0.005;
    static char C_0 = 1;
    static float Q_bias, Angle_err;
    static float PCt_0, PCt_1, E;
    static float K_0, K_1, t_0, t_1;
    static float Pdot[4] ={0,0,0,0};
    static float PP[2][2] = { { 1, 0 },{ 0, 1 } };

/*    GESTURE_t fileteredRestult;
    MPU_RESULT_U_t result = convert_mpu_result();
*/
    double Accel = atan2(info->AccX, info->AccZ)*180/3.141593653; // calculate angle from accelor//计算倾角
    //printf("%d,", (int)(Accel*1000));
    double Gyro = -(info->GroY/16.384);                                    // convert gyroscope result
    angle += (Gyro - Q_bias) * dt; //先验估计
    //printf("%d,", (int)(angle*1000));
    Pdot[0] = Q_angle - PP[0][1] - PP[1][0]; // Pk-先验估计误差协方差的微分

    Pdot[1] = -PP[1][1];
    Pdot[2] = -PP[1][1];
    Pdot[3] = Q_gyro;
    PP[0][0] += Pdot[0] * dt;   // Pk-先验估计误差协方差微分的积分
    PP[0][1] += Pdot[1] * dt;   // =先验估计误差协方差
    PP[1][0] += Pdot[2] * dt;
    PP[1][1] += Pdot[3] * dt;
        
    Angle_err = Accel - angle;  //zk-先验估计
    
    PCt_0 = C_0 * PP[0][0];
    PCt_1 = C_0 * PP[1][0];
    
    E = R_angle + C_0 * PCt_0;
    
    K_0 = PCt_0 / E;
    K_1 = PCt_1 / E;
    
    t_0 = PCt_0;
    t_1 = C_0 * PP[0][1];

    PP[0][0] -= K_0 * t_0;       //后验估计误差协方差
    PP[0][1] -= K_0 * t_1;
    PP[1][0] -= K_1 * t_0;
    PP[1][1] -= K_1 * t_1;
        
    angle   += K_0 * Angle_err;  //后验估计
    Q_bias  += K_1 * Angle_err;  //后验估计
    angle_dot   = Gyro - Q_bias;     //输出值(后验估计)的微分=角速度
    //printf("%d\n", (int)(angle*1000));
    pos->Angle = angle;
    pos->GroY = Gyro;
}

int GetAnglePID(double angle, float Gyro)
{
    float Bias;
    const float kp=180, kd=3;//180,3
    int balance_pwm;
    Bias = angle - BALANCE_ANGLE;
    // PD control, P times bias, D times angle change, palstance
    balance_pwm = kp*Bias + Gyro*kd;
    return balance_pwm;
}

#if 0
int velocity(int encoder_left, int encoder_right)
{
    static float Velocity, Encoder_Least, Encoder, Movement;
    static float Encoder_Integral, Target_Velocity;
    float Kp = 30, ki=Kp/200;//
    Movement = 0;
    // Velocity PI controller
    
    // least speed difference = car speed, here is 0
    Encoder_Least = (encoder_left + encoder_right) - 0;
    // low pass filter 
    Encoder *= 0.7;
    Encoder += Encoder_Least * 0.3;
    // speed times time, got distance
    Encoder_Integral += Encoder;
    // substact movement if car moves
    Encoder_Integral = Encoder_Integral - Movement;
    // amplitude limitation for integral
    Encoder_Integral = (Encoder_Integral > 15000) ? 15000 : Encoder_Integral;
    Encoder_Integral = (Encoder_Integral < -15000) ? -15000 : Encoder_Integral;
    // PI loop for new velocity change.
    Velocity = Encoder * Kp + Encoder_Integral * ki;
    return Velocity;
}
#endif

int GetSpeedPit(int encCurSpeed, int encTargetSpeed, int encMovement)
{
    static float sEncLastSpeed, sEncIntegral;
    const float Kp = 30, ki = Kp/200; /* 这里的比例系数同PWM也有对应关系 */
    int speed_pwm;
    
    //Movement = 0;
    // Velocity PI controller
    
    /* 更新最近一次的速度，使用了低通滤波器 */
    sEncLastSpeed = (sEncLastSpeed * 0.7) + (encCurSpeed * 0.3); /* 低通滤波器 */

    /* 对编码器的值进行累加，获得积分 */
    sEncIntegral = sEncIntegral + sEncLastSpeed - encMovement; /* 累加位移相对于目标的差值 */

    /* 限制变化的速度 */
    sEncIntegral = (sEncIntegral > 15000) ? 15000 : sEncIntegral;
    sEncIntegral = (sEncIntegral < -15000) ? -15000 : sEncIntegral;
    
    // PI loop for new velocity change.
    speed_pwm = (int)(sEncLastSpeed * Kp + sEncIntegral * ki);

    return speed_pwm;
}

