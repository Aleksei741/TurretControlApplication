#pragma once
#ifndef _MAIN_H // Блокируем повторное включение этого модуля
#define _MAIN_H
//******************************************************************************
// Секция include
//******************************************************************************
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>

#include <Windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <strsafe.h>
#include <chrono>
#include "resource.h"


#pragma warning(disable: 4996)

#pragma comment(lib, "comctl32.lib")

#include "GUIDefinition.h"
#include "File.h"
#include "CommandSend.h"
#include "KeyHook.h"
#include "video_H264_decode.h"
#include "ImgProcessor.h"
#include "MouseProcess.h"
#include "WindowVideoOption.h"
#include "WindowControlOption.h"
#include "WindowDamageOption.h"
#include "WindowAimOption.h"
#include "WindowIndicateOption.h"

// Enable Visual Style

//******************************************************************************
// Секция определения констант
//******************************************************************************
#define DEBUG
#define WIN32_LEAN_AND_MEAN

#define PORT_COMMAND 20101
#define IP_COMMAND "127.0.0.1"

#define PORT_RTP_H264 20100
#define IP_RTP_H264 "0.0.0.0"

typedef enum ControlSource_dType {
	KEYBOARD,
	MOUSE
};
//******************************************************************************
// Секция определения типов
//******************************************************************************
typedef struct
{
	BOOL Active;
	UINT Offset;
	UINT LenLine;
	TCHAR Text[40];
}AimText_DType;

typedef struct
{	
	UINT X;
	UINT Y;
	UINT width;
	UINT height;
	UINT LineWidthPix;
	UINT TextWidthPix;
	UINT TextHeightPix;
	AimText_DType AimText[3];
	COLORREF TextColor;
}AimOption_DType;

typedef struct
{
	BOOL fSendReqParam;
	BOOL fRecv;
	BOOL fSend;

	UINT DelaySensor_ms;

	UINT DamageDelayMinute;
	UINT DamageDelaySecond;
	UINT HealPoint;

	BOOL MovementOff;
	BOOL WeaponOff;
	BOOL VideoOff;
}DamageOption_DType;

typedef struct
{
	UINT MaxStepsPosition;
	UINT MinStepsPosition;
	UINT MaxDegreePosition;	//Ограничение вращения
	UINT MinDegreePosition;
	float RotationSpeed;	//Скорость вращения
	//UINT StepsStepperMotor;	//Шагов шагового двигателя
	UINT MicroStepsStepperMotor;	//Шагов шагового двигателя
	float ReductionRatioStepperMotor;	//Коэффициегт редукции
	UINT Freq;	//Частота управляющего сигнала
	float Period;	//Период
}ControlMotor_DType;

typedef struct
{
	BOOL fSendReqParam;
	BOOL fRecv;
	BOOL fSend;

	BOOL FlagNoLimitStepMotor;
	BOOL FlagZeroPosition;

	ControlMotor_DType M1;
	ControlMotor_DType M2;

	float MouseCoefficient;
}ControlOption_DType;

typedef struct
{
	UINT weight;	// 
	UINT height;  //
	UINT VideoPort; //
	clock_t bitrate;
	BOOL framerate; //
	TCHAR IP[30];
}VideoOption_DType;

typedef struct
{
	UINT LineWidthPix;
	UINT TextWidthPix;
	UINT TextHeightPix;
	COLORREF Color;
}IndicateOption_DType;

struct UserParameters_DType
{
	UCHAR VideoActive;
	ControlSource_dType CotrolSource;
	CHAR HealPoint;
	clock_t timeDemage;

	INT PositionM1;
	INT PositionM2;
	INT NeedPositionM1;
	INT NeedPositionM2;

	VideoOption_DType VideoOption;
	ControlOption_DType ControlOption;
	DamageOption_DType DamageOption;
	AimOption_DType AimOption;
	IndicateOption_DType IndicateOption;
};
//******************************************************************************
// Секция определения глобальных переменных
//******************************************************************************
extern HWND hwndMainWindow;
extern HWND hwndWideo;
extern UserParameters_DType param;
//******************************************************************************
// Секция прототипов глобальных функций
//******************************************************************************

//******************************************************************************
// Секция определения макросов
//******************************************************************************
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))
#endif
//******************************************************************************
// ENF OF FILE
//******************************************************************************