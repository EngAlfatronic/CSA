#include "main.h"

#ifndef STRUCT_PID

	#define STRUCT_PID
	typedef struct
	{
			float Kp, Ki, Kd;  // Ganhos
			float error, last_error, integral, derivative, integral_limit, output_limit;
			float output;
	} PID_Controller;
#endif
	
void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd, float integral_limit, float output_limit);
float PID_Update(PID_Controller *pid, float setpoint, float input, float dt);
