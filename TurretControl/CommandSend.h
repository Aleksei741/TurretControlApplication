#pragma once
#ifndef _CONTROL_SEND_H // ��������� ��������� ��������� ����� ������
#define _CONTROL_SEND_H
//******************************************************************************
// ������ include
//******************************************************************************
#include "main.h"
#include <chrono>
#include <Windows.h>
//******************************************************************************
// ������ ����������� ��������
//******************************************************************************

//******************************************************************************
// ������ ����������� �����
//******************************************************************************
typedef struct
{
	int val;
	clock_t time;
}DamageStore_DType;

typedef struct
{
	int motor1;
	int motor2;
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
// ������ ����������� ���������� ����������
//******************************************************************************

//******************************************************************************
// ������ ���������� ���������� �������
//******************************************************************************
void SendCommandProcessStart(LPWSTR strIP);
void SetIPCommand(LPWSTR strIP);
void SendCommandProcessStop(void);
BOOL GetComandConectionStatus(void);
void SetComandButton(BOOL fLeft, BOOL fRight, BOOL fUp, BOOL fDown, BOOL fAttack);
void SetComandMouse(INT lr, INT ud, BOOL fAttack);
extern void (*CallbackComandConectionStatus)(BOOL index);
extern void (*CallbackVideoStatus)(BOOL status);
extern void (*CallbackHPStatus)(INT index);
BOOL SendCommandVideoON(void);
BOOL SendCommandVideoOFF(void);
BOOL DamageReset(void);
//******************************************************************************
// ������ ����������� ��������
//******************************************************************************

#endif
//******************************************************************************
// ENF OF FILE
//******************************************************************************