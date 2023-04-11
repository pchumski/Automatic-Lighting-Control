/*
 * regulator.c
 *
 *  Created on: Jan 15, 2022
 *      Author: Konstanty
 */
#include "regulator.h"

float Reg_SignalControl(regulator_Handle_TypeDef* Reg,float y_ref, float pomiar)
{
	float e;
	float u;
	float u_sat;
	float N=0.01f;
	e = y_ref - pomiar;
			Reg->e_int += Reg->Ki*Reg->Ts*e;
			Reg->e_der = (Reg->Kd*N)*(e-Reg->e_prev) + (1.0f - N*Reg->Ts)*Reg->e_der;
			Reg->e_prev = e;

			u = Reg->e_int + Reg->e_der + (Reg->Kp*e);

			if(u > Reg->limitup)
			{
			  u_sat = Reg->limitup;
			}
			else if(u < Reg->limitdown)
			{
			  u_sat = Reg->limitdown;
			}
			else
			{
			  u_sat = u;
			}

			if(u!=u_sat)
			{
			  Reg->e_int -=Reg->Ki*Reg->Ts*e;
			}
	return u_sat;
};

