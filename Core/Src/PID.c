/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Produto CSA
// Autor  Iuri Lima
// Versaon V1.0
// Date 11/07/2025
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "main.h"
#include "mpu_6050.h"
#include "perifericos.h"
#include "flash.h"
#include "structs.h"
#include "PID.h"
#include <math.h>

void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float integral_limit, float output_limit) 
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    pid->output = 0.0f;
    pid->integral_limit = integral_limit;
    pid->output_limit = output_limit;
}

float PID_Update(PID_Controller *pid, float setpoint, float input, float dt) 
{
		if(habil_pid == 0 || dt == 0)return pid->output;
	
    pid->error = setpoint - input;  // Erro = 0 - angle

    // Termo Proporcional (sempre reage ao erro)
    float proportional = pid->Kp * pid->error;

    // Termo Integral (com anti-windup)
    pid->integral += pid->error * dt;
    pid->integral = fmaxf(fminf(pid->integral, pid->integral_limit), -pid->integral_limit);

    // Termo Derivativo
    pid->derivative = (pid->error - pid->last_error) / dt;

    // Sa�da do PID
    pid->output = proportional + (pid->Ki * pid->integral) + (pid->Kd * pid->derivative);
    pid->output = fmaxf(fminf(pid->output, pid->output_limit), -pid->output_limit);

    pid->last_error = pid->error;
    return pid->output;
}
