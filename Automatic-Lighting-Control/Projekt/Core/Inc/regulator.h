/*
 * regulator.h
 *
 *  Created on: Jan 15, 2022
 *      Author: Konstanty
 */

#ifndef INC_REGULATOR_H_
#define INC_REGULATOR_H_



#define reg_Type regulator_Handle_TypeDef*

typedef struct{
	 float Ki;
	 float Kd;
	 float Kp;
	 float Ts;
	 float e_int;
	 float e_der;
	 float e_prev;
	 float limitdown;
	 float limitup;
 }regulator_Handle_TypeDef;


 float Reg_SignalControl(regulator_Handle_TypeDef* Reg,float y_ref, float pomiar);




#endif /* INC_REGULATOR_H_ */
