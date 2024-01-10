#pragma once
#ifndef _CONTROL_SEND_H // Блокируем повторное включение этого модуля
#define _CONTROL_SEND_H
//******************************************************************************
// Секция include
//******************************************************************************
#include "main.h"
#include <chrono>
#include <Windows.h>
//******************************************************************************
// Секция определения констант
//******************************************************************************

//******************************************************************************
// Секция определения типов
//******************************************************************************
typedef struct
{
	int val;
	clock_t time;
}DamageStore_DType;

typedef struct
{
	int motor1;
	clock_t timeM1Action;
	int motor2;
	clock_t timeM2Action;
	int trigger;
}MotorCommand_DType;

typedef struct {
	unsigned char* data;
	int size;
	bool* flag;
}CommandData_DType;

typedef enum TransmiteMode_DType
{
	TX_SOCKET_OFF,
	TX_SOCKET_SET_PARAM,
	TX_SOCKET_COMMAND,
	TX_SOCKET_VIDEO_ON,
	TX_SOCKET_VIDEO_OFF,
};
//******************************************************************************
// Секция определения глобальных переменных
//******************************************************************************

//******************************************************************************
// Секция прототипов глобальных функций
//******************************************************************************
void SendCommandProcessStart(LPWSTR strIP);
void SetIPCommand(LPWSTR strIP);
void SendCommandProcessStop(void);
BOOL GetComandConectionStatus(void);
void SetComand(BOOL fLeft, BOOL fRight, BOOL fUp, BOOL fDown, BOOL fAttack);
extern void (*CallbackComandConectionStatus)(BOOL index);
extern void (*CallbackVideoStatus)(BOOL status);
extern void (*CallbackHPStatus)(INT index, clock_t time);
BOOL SendCommandVideoON(void);
BOOL SendCommandVideoOFF(void);
BOOL DamageReset(void);
//******************************************************************************
// Секция определения макросов
//******************************************************************************

#endif
//******************************************************************************
// ENF OF FILE
//******************************************************************************