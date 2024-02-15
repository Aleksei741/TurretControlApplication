#pragma once
#ifndef _MOUSE_HOOK_H // ��������� ��������� ��������� ����� ������
#define _MOUSE_HOOK_H
//******************************************************************************
// ������ include
//******************************************************************************
#include <windows.h>
//******************************************************************************
// ������ ����������� ��������
//******************************************************************************

//******************************************************************************
// ������ ����������� �����
//******************************************************************************
typedef struct _MYHOOKDATA
{
    int nType;
    HOOKPROC hkprc;
    HHOOK hhook;
} MYHOOKDATA;

struct MOUSE_STATUStypedef {
	INT xPosition;
	INT yPosition;
	BOOL LeftButton;
	BOOL RightButoon;
};
//******************************************************************************
// ������ ����������� ���������� ����������
//******************************************************************************

//******************************************************************************
// ������ ���������� ���������� �������
//******************************************************************************
UCHAR MouseProcess_init(void);
UCHAR MouseProcess_deinit(void);
UCHAR MouseProcess_start(HWND hwndMain);
UCHAR MouseProcess_stop(void);
extern void (*MouseHookInterruptProcessing)(INT lr, INT ud, BOOL fAttack);
//******************************************************************************
// ������ ����������� ��������
//******************************************************************************

#endif
//******************************************************************************
// ENF OF FILE
//******************************************************************************
