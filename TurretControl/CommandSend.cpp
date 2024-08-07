//******************************************************************************
//include
//******************************************************************************
#include "main.h"
#include <chrono>
#include <queue>
#include <list>
#include "CommandSend.h"
//******************************************************************************
// ������ ����������� ����������, ������������ � ������
//******************************************************************************
//------------------------------------------------------------------------------
// ����������
//------------------------------------------------------------------------------
void (*CallbackComandConectionStatus)(BOOL index);
void (*CallbackVideoStatus)(BOOL index);
void (*CallbackHPStatus)(INT index);
//------------------------------------------------------------------------------
// ���������
//------------------------------------------------------------------------------
HANDLE hMutexSendCommand;
HANDLE hTreadSendCommandProcedure;
HANDLE hTreadTicProcedure;

TransmiteMode_DType TransmiteMode = TX_SOCKET_OFF;
MotorCommand_DType MotorCommand;
BOOL statusConetcion;

static std::queue<CommandData_DType> QParameters;
static char CommandMassage[12] = { 'T', 'C', 0, 0, 0 ,0, 0, 0, 0, 0, 0xA5, 0xA5 };
static char ReciveMassage[30] = { 0 };
SOCKET SOCKETConnection;

static char chIP[30];
//******************************************************************************
// ������ ���������� ��������� �������
//******************************************************************************
DWORD WINAPI SendCommandProcedure(CONST LPVOID lpParam);
DWORD WINAPI TicProcedure(CONST LPVOID lpParam);
void FillMotionCommand(void);
void FillParamMassage(void);
BOOL SetControlOption(void);
void ParseInputData(void);
BOOL RequestControlOption(void);
BOOL RequestSensorOption(void);
BOOL SetSensorOption(void);
//******************************************************************************
// ������ �������� �������
//******************************************************************************
void SendCommandProcessStart(LPWSTR strIP)
{
	size_t nNumCharConverted;
	wcstombs_s(&nNumCharConverted, chIP, 30, strIP, 30);

	TransmiteMode = TX_SOCKET_COMMAND;

	hMutexSendCommand = CreateMutex(
		NULL,              // default security attributes
		FALSE,             // initially not owned
		NULL);             // unnamed mutex
	hTreadSendCommandProcedure = CreateThread(NULL, NULL, SendCommandProcedure, NULL, NULL, NULL);
	hTreadTicProcedure = CreateThread(NULL, NULL, TicProcedure, NULL, NULL, NULL);
}
//------------------------------------------------------------------------------
void SetIPCommand(LPWSTR strIP)
{
	size_t nNumCharConverted;
	wcstombs_s(&nNumCharConverted, chIP, 30, strIP, 30);
}
//------------------------------------------------------------------------------
void SendCommandProcessStop(void)
{
	CommandData_DType massage;

	TransmiteMode = TX_SOCKET_OFF;
	
	if (hMutexSendCommand) CloseHandle(hMutexSendCommand);
	if (hTreadSendCommandProcedure) CloseHandle(hTreadSendCommandProcedure);
	if (hTreadTicProcedure) CloseHandle(hTreadTicProcedure);
	closesocket(SOCKETConnection);

	while (!QParameters.empty())
	{
		massage = QParameters.front();
		QParameters.pop();
		free(massage.data);
	}
}
//------------------------------------------------------------------------------
DWORD WINAPI TicProcedure(CONST LPVOID lpParam)
{
	while (1)
	{
		Sleep(100);
				
		//������� ���������� ���������� ������
		if (param.ControlOption.fSendReqParam)
		{
			if (RequestControlOption())
				param.ControlOption.fSendReqParam = FALSE;
		}

		//��������� ���������� ���������� ������
		if (param.ControlOption.fSend)
		{
			if (SetControlOption())
				param.ControlOption.fSend = FALSE;
		}
		//������� ���������� �������
		if (param.DamageOption.fSendReqParam)
		{
			if (RequestSensorOption())
				param.DamageOption.fSendReqParam = FALSE;
		}

		//��������� ���������� �������
		if (param.DamageOption.fSend)
		{
			if (SetSensorOption())
				param.DamageOption.fSend = FALSE;
		}


		//��������� �������� ��� ������ � �����������
		if(param.CotrolSource == KEYBOARD)
			ResumeThread(hTreadSendCommandProcedure);
	}
}
//------------------------------------------------------------------------------
DWORD WINAPI SendCommandProcedure(CONST LPVOID lpParam)
{
	SOCKADDR_IN addr;
	int sizeofaddr;
	static TransmiteMode_DType fLastTXMode;
	char chIPBuffer[30];
	int ret;
	
	while (1)
	{
		sizeofaddr = sizeof(addr);
		addr.sin_addr.s_addr = inet_addr(chIP);
		addr.sin_port = htons(PORT_COMMAND);
		addr.sin_family = AF_INET;
		SOCKETConnection = socket(AF_INET, SOCK_STREAM, NULL);

		memcpy(chIPBuffer, chIP, sizeof(chIP));
		ret = connect(SOCKETConnection, (SOCKADDR*)&addr, sizeof(addr));

		while (ret == 0)
		{
			//���� �������� IP
			if (strncmp(chIP, chIPBuffer, sizeof(chIP)) != 0)
				break;

			//�������� �������
			WaitForSingleObject(hMutexSendCommand, 100);			
			if (QParameters.empty())
			{
				FillMotionCommand();
			}
			else
			{
				FillParamMassage();
			}
			ReleaseMutex(hMutexSendCommand);
			
			//��������
			if (SOCKET_ERROR == send(SOCKETConnection, CommandMassage, sizeof(CommandMassage), NULL))
			{
				//error
				closesocket(SOCKETConnection);
				SOCKETConnection = socket(AF_INET, SOCK_STREAM, NULL);
				connect(SOCKETConnection, (SOCKADDR*)&addr, sizeof(addr));
				statusConetcion = FALSE;
				memset(ReciveMassage, 0, sizeof(ReciveMassage));
			}
			else
			{
				statusConetcion = TRUE;
				recv(SOCKETConnection, ReciveMassage, sizeof(ReciveMassage), NULL);
			}

			//��������� ReciveMassage
			ParseInputData();

			//����������� ������� ���������� 
			if (CallbackComandConectionStatus) CallbackComandConectionStatus(statusConetcion);

			//�����
			if (QParameters.empty())
				SuspendThread(hTreadSendCommandProcedure);
			else if (statusConetcion == FALSE)
				Sleep(250);
			else
				Sleep(10);
		}
		closesocket(SOCKETConnection);
		Sleep(500);
	}
	return NULL;
}
//------------------------------------------------------------------------------
BOOL DamageReset(void)
{
	CommandData_DType massage;
	static bool TPResetHP = FALSE;

	if (!statusConetcion)
		return FALSE;

	//--------------------------------------------------------
	if (!TPResetHP)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &TPResetHP;
		*massage.flag = TRUE;

		massage.data[2] = 'S';	//Sensor
		massage.data[3] = 'F';	//Flag
		massage.data[4] = 'R';	//ResetDamage
		massage.data[5] = 0;	//Write
		massage.data[6] = 1;
		massage.data[7] = 0;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	else
		return FALSE;

	return TRUE;
}
//------------------------------------------------------------------------------
BOOL GetComandConectionStatus(void)
{
	return statusConetcion;
}
//------------------------------------------------------------------------------
void SetComandButton(BOOL fLeft, BOOL fRight, BOOL fUp, BOOL fDown, BOOL fAttack)
{
	int lr;
	int ud;

	if (param.CotrolSource != KEYBOARD)
		return;

	//���������� ������� ������ 
	if (fLeft)
		lr = -32000;
	else if (fRight)
		lr = 32000;
	else
		lr = 0;

	//������ ��������
	if (param.HealPoint <= 0 && param.DamageOption.MovementOff)
	{
		MotorCommand.motor1 = 0;
	}
	else
	{
		MotorCommand.motor1 = lr;
	}

	//���������� ������� ������ 
	if (fUp)
		ud = -32000;
	else if (fDown)
		ud = 32000;
	else
		ud = 0;

	//������ ��������
	if (param.HealPoint <= 0 && param.DamageOption.MovementOff)
	{
		MotorCommand.motor2 = 0;
	}
	else
	{
		MotorCommand.motor2 = ud;
	}
	
	if (param.HealPoint <= 0 && param.DamageOption.WeaponOff)
		MotorCommand.trigger = 0;
	else
		MotorCommand.trigger = fAttack;

	if (TransmiteMode == TX_SOCKET_COMMAND)
	{		
		ResumeThread(hTreadSendCommandProcedure);
	}
}
//------------------------------------------------------------------------------
void SetComandMouse(INT lr, INT ud, BOOL fAttack)
{
	if (param.CotrolSource != MOUSE)
		return;

	//������ ��������
	if (param.HealPoint <= 0 && param.DamageOption.MovementOff)
	{
		MotorCommand.motor1 = 0;
	}
	else
	{
		MotorCommand.motor1 = lr;
	}

	//������ ��������
	if (param.HealPoint <= 0 && param.DamageOption.MovementOff)
	{
		MotorCommand.motor2 = 0;
	}
	else
	{
		MotorCommand.motor2 = ud;
	}

	if (param.HealPoint <= 0 && param.DamageOption.WeaponOff)
		MotorCommand.trigger = 0;
	else
		MotorCommand.trigger = fAttack;

	if (TransmiteMode == TX_SOCKET_COMMAND)
	{
		ResumeThread(hTreadSendCommandProcedure);
	}
}
//------------------------------------------------------------------------------
void FillMotionCommand(void)
{	
	CommandMassage[0] = 'T';
	CommandMassage[1] = 'C';
	CommandMassage[2] = 'C';
	CommandMassage[3] = MotorCommand.motor1;
	CommandMassage[4] = MotorCommand.motor1 >> 8;
	CommandMassage[5] = MotorCommand.motor2;
	CommandMassage[6] = MotorCommand.motor2 >> 8;
	CommandMassage[7] = MotorCommand.trigger;
	CommandMassage[8] = 0;
	CommandMassage[9] = 0;
	CommandMassage[10] = 0xA5;
	CommandMassage[11] = 0xA5;
}
//------------------------------------------------------------------------------
void FillParamMassage(void)
{
	CommandData_DType massage;

	massage = QParameters.front();
	QParameters.pop();

	memcpy(CommandMassage, massage.data, massage.size);
	*massage.flag = FALSE;

	CommandMassage[0] = 'T';
	CommandMassage[1] = 'C';	
	CommandMassage[10] = 0xA5;
	CommandMassage[11] = 0xA5;

	free(massage.data);
}
//------------------------------------------------------------------------------
BOOL SendCommandVideoON(void)
{
	CommandData_DType massage;
	static bool VPHeight = FALSE;
	static bool VPWeight = FALSE;
	static bool VPBitrate = FALSE;
	static bool VPFramerate = FALSE;
	static bool VPPort = FALSE;
	static bool VStart = FALSE;

	if (!statusConetcion)
		return FALSE;

	//--------------------------------------------------------	
	if (!VPHeight)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VPHeight;
		*massage.flag = TRUE;

		massage.data[2] = 'V';	//Video
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'H';	//Height
		massage.data[5] = param.VideoOption.height & 0xFF;
		massage.data[6] = (param.VideoOption.height >> 8) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!VPWeight)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VPWeight;
		*massage.flag = TRUE;

		massage.data[2] = 'V';	//Video
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'W';	//Weight
		massage.data[5] = param.VideoOption.weight & 0xFF;
		massage.data[6] = (param.VideoOption.weight >> 8) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!VPBitrate)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VPBitrate;
		*massage.flag = TRUE;

		massage.data[2] = 'V';	//Video
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'B';	//Bitrate
		massage.data[5] = param.VideoOption.bitrate & 0xFF;
		massage.data[6] = (param.VideoOption.bitrate >> 8) & 0xFF;
		massage.data[7] = (param.VideoOption.bitrate >> 16) & 0xFF;
		massage.data[8] = (param.VideoOption.bitrate >> 24) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!VPFramerate)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VPFramerate;
		*massage.flag = TRUE;

		massage.data[2] = 'V';	//Video
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'F';	//Framerate
		massage.data[5] = param.VideoOption.framerate & 0xFF;
		massage.data[6] = (param.VideoOption.framerate >> 8) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!VPPort)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VPPort;
		*massage.flag = TRUE;

		massage.data[2] = 'V';	//Video
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'P';	//Port
		massage.data[5] = param.VideoOption.VideoPort & 0xFF;
		massage.data[6] = (param.VideoOption.VideoPort >> 8) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!VStart)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VStart;
		*massage.flag = TRUE;

		massage.data[2] = 'V';	//Video
		massage.data[3] = 'S';	//Start/Stop
		massage.data[4] = 1;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------

	ResumeThread(hTreadSendCommandProcedure);
	return TRUE;

}
//------------------------------------------------------------------------------
BOOL SendCommandVideoOFF(void)
{
	CommandData_DType massage;
	static bool VStop = FALSE;

	if (!statusConetcion)
		return FALSE;

	//--------------------------------------------------------
	if (!VStop)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VStop;
		*massage.flag = TRUE;

		massage.data[2] = 'V';	//Video
		massage.data[3] = 'S';	//Start/Stop
		massage.data[4] = 0;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------

	ResumeThread(hTreadSendCommandProcedure);
	return TRUE;
}
//------------------------------------------------------------------------------
void ParseInputData(void)
{
	unsigned int value;
	int i;
	INT HealPoint;

	if (ReciveMassage[0] != 0x54 && ReciveMassage[1] != 0x43) //Turret Control
		return;

	//=================================================================================================================
	//������� ���������	
	if (ReciveMassage[2] == 0x57) //����� �������� ������
	{
		//����������� ������� ������ �������� ����� 
		if (CallbackVideoStatus) CallbackVideoStatus(ReciveMassage[21]);
		//������� HP
		//-----------------------------------------------------------------------------------
		param.HealPoint = ReciveMassage[19];
		//-----------------------------------------------------------------------------------
		//����� HP �� �������		
		if (CallbackHPStatus) CallbackHPStatus(param.HealPoint);
		//-----------------------------------------------------------------------------------
		//������� ������
		unsigned char buf = ReciveMassage[3];
		param.PositionM1 = buf;
		param.PositionM1 |= (unsigned char)ReciveMassage[4] << 8;
		param.PositionM1 |= (unsigned char)ReciveMassage[5] << 16;
		param.PositionM1 |= (unsigned char)ReciveMassage[6] << 24;

		param.PositionM2 = (unsigned char)ReciveMassage[7];
		param.PositionM2 |= ((unsigned char)ReciveMassage[8] << 8);
		param.PositionM2 |= ((unsigned char)ReciveMassage[9] << 16);
		param.PositionM2 |= ((unsigned char)ReciveMassage[10] << 24);

		param.NeedPositionM1 = (unsigned char)ReciveMassage[11];
		param.NeedPositionM1 |= ((unsigned char)ReciveMassage[12] << 8);
		param.NeedPositionM1 |= ((unsigned char)ReciveMassage[13] << 16);
		param.NeedPositionM1 |= ((unsigned char)ReciveMassage[14] << 24);

		param.NeedPositionM2 = (unsigned char)ReciveMassage[15];
		param.NeedPositionM2 |= ((unsigned char)ReciveMassage[16] << 8);
		param.NeedPositionM2 |= ((unsigned char)ReciveMassage[17] << 16);
		param.NeedPositionM2 |= ((unsigned char)ReciveMassage[18] << 24);
	}
	//=================================================================================================================
	//��������� ����������
	if (ReciveMassage[2] == 0x56) //Movement 
	{
		if (ReciveMassage[3] == 0x50) //Parameters
		{
			value = (unsigned char)ReciveMassage[5] | ((unsigned char)ReciveMassage[6] << 8) | ((unsigned char)ReciveMassage[7] << 16) | ((unsigned char)ReciveMassage[8] << 24);
			if (ReciveMassage[4] == 0x01) //MaxSteppersStepMotor1
			{
				param.ControlOption.M1.MaxStepsPosition = value;
				param.ControlOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x02) //MaxSteppersStepMotor2
			{
				param.ControlOption.M2.MaxStepsPosition = value;
				param.ControlOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x03) //MinSteppersStepMotor1
			{
				param.ControlOption.M1.MinStepsPosition = value;
				param.ControlOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x04) //MinSteppersStepMotor2
			{
				param.ControlOption.M2.MinStepsPosition = value;
				param.ControlOption.fRecv = TRUE;
			}
			if (ReciveMassage[4] == 0x03) //MinSteppersStepMotor1
			{
				param.ControlOption.M1.MinStepsPosition = value;
				param.ControlOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x04) //MinSteppersStepMotor2
			{
				param.ControlOption.M2.MinStepsPosition = value;
				param.ControlOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x11) //FreqMotor1
			{
				param.ControlOption.M1.Freq = value;
				param.ControlOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x12) //FreqMotor2
			{
				param.ControlOption.M2.Freq = value;
				param.ControlOption.fRecv = TRUE;
			}
		}
		else if (ReciveMassage[3] == 0x46) //Flag
		{
			value = ReciveMassage[5];
			if (ReciveMassage[4] == 0x4C) //FlagNoLimitStepMotor
			{
				param.ControlOption.FlagNoLimitStepMotor = value;
				param.ControlOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x5A) //FlagZeroPosition
			{
				param.ControlOption.FlagZeroPosition = value;
				param.ControlOption.fRecv = TRUE;
			}
		}
	}
	//=================================================================================================================
	//��������� ��������� �����
	if (ReciveMassage[2] == 0x53) //Sensor
	{
		if (ReciveMassage[3] == 0x50) //Parameters
		{
			value = (unsigned char)ReciveMassage[5] | ((unsigned char)ReciveMassage[6] << 8) | ((unsigned char)ReciveMassage[7] << 16) | ((unsigned char)ReciveMassage[8] << 24);
			if (ReciveMassage[4] == 0x44) //DelaySensor
			{
				param.DamageOption.DelaySensor_ms = value;
				param.DamageOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x48) //HP
			{
				param.DamageOption.HealPoint = value;
				param.DamageOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x4D) //Minutes Delay
			{
				param.DamageOption.DamageDelayMinute = value;
				param.DamageOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x53) //Seconds Delay
			{
				param.DamageOption.DamageDelaySecond = value;
				param.DamageOption.fRecv = TRUE;
			}
		}	
	}
}
//------------------------------------------------------------------------------
BOOL RequestControlOption(void)
{
	CommandData_DType massage;
	static bool VMaxSteppersStepM1 = FALSE;
	static bool VMaxSteppersStepM2 = FALSE;
	static bool VMinSteppersStepM1 = FALSE;
	static bool VMinSteppersStepM2 = FALSE;
	static bool VFreqM1 = FALSE;
	static bool VFreqM2 = FALSE;
	static bool VFlagNoLimit = FALSE;
	static bool VFlagZero = FALSE;

	if (!statusConetcion)
		return FALSE;

	//--------------------------------------------------------	
	if (!VMaxSteppersStepM1)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VMaxSteppersStepM1;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x01;	//Max steppers step motor1
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!VMaxSteppersStepM2)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VMaxSteppersStepM2;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x02;	//Max steppers step motor2
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!VMinSteppersStepM1)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VMinSteppersStepM1;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x03;	//Max steppers step motor1
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!VMinSteppersStepM2)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VMinSteppersStepM2;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x04;	//Max steppers step motor2
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!VFreqM1)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VFreqM1;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x11;	//FreqMotor1
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!VFreqM2)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VFreqM2;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x12;	//FreqMotor1
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!VFlagNoLimit)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VFlagNoLimit;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'F';	//Flag
		massage.data[4] = 'F';	//ZeroPosition
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!VFlagZero)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VFlagZero;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'F';	//Flag
		massage.data[4] = 'L';	//NoLimitStepMotor
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------

	ResumeThread(hTreadSendCommandProcedure);
	return TRUE;
}
//------------------------------------------------------------------------------
BOOL SetControlOption(void)
{
	CommandData_DType massage;
	static bool VMaxSteppersStepM1 = FALSE;
	static bool VMaxSteppersStepM2 = FALSE;
	static bool VMinSteppersStepM1 = FALSE;
	static bool VMinSteppersStepM2 = FALSE;
	static bool VFreqM1 = FALSE;
	static bool VFreqM2 = FALSE;
	static bool VFlagNoLimit = FALSE;
	static bool VFlagZero = FALSE;

	if (!statusConetcion)
		return FALSE;

	//--------------------------------------------------------	
	if (!VMaxSteppersStepM1)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VMaxSteppersStepM1;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x01;	//Max steppers step motor1
		massage.data[5] = 0; //Write
		massage.data[6] = param.ControlOption.M1.MaxStepsPosition & 0xFF;
		massage.data[7] = (param.ControlOption.M1.MaxStepsPosition >> 8) & 0xFF;
		massage.data[8] = (param.ControlOption.M1.MaxStepsPosition >> 16) & 0xFF;
		massage.data[9] = (param.ControlOption.M1.MaxStepsPosition >> 24) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!VMaxSteppersStepM2)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VMaxSteppersStepM2;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x02;	//Max steppers step motor2
		massage.data[5] = 0; //Write
		massage.data[6] = param.ControlOption.M2.MaxStepsPosition & 0xFF;
		massage.data[7] = (param.ControlOption.M2.MaxStepsPosition >> 8) & 0xFF;
		massage.data[8] = (param.ControlOption.M2.MaxStepsPosition >> 16) & 0xFF;
		massage.data[9] = (param.ControlOption.M2.MaxStepsPosition >> 24) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!VMinSteppersStepM1)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VMinSteppersStepM1;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x03;	//Min steppers step motor1
		massage.data[5] = 0; //Write
		massage.data[6] = param.ControlOption.M1.MinStepsPosition & 0xFF;
		massage.data[7] = (param.ControlOption.M1.MinStepsPosition >> 8) & 0xFF;
		massage.data[8] = (param.ControlOption.M1.MinStepsPosition >> 16) & 0xFF;
		massage.data[9] = (param.ControlOption.M1.MinStepsPosition >> 24) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!VMinSteppersStepM2)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VMinSteppersStepM2;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x04;	//Min steppers step motor2
		massage.data[5] = 0; //Write
		massage.data[6] = param.ControlOption.M2.MinStepsPosition & 0xFF;
		massage.data[7] = (param.ControlOption.M2.MinStepsPosition >> 8) & 0xFF;
		massage.data[8] = (param.ControlOption.M2.MinStepsPosition >> 16) & 0xFF;
		massage.data[9] = (param.ControlOption.M2.MinStepsPosition >> 24) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!VFreqM1)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VFreqM1;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x11;	//FreqMotor1
		massage.data[5] = 0; //Write
		massage.data[6] = param.ControlOption.M1.Freq & 0xFF;
		massage.data[7] = (param.ControlOption.M1.Freq >> 8) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!VFreqM2)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VFreqM2;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 0x12;	//FreqMotor1
		massage.data[5] = 0; //Write
		massage.data[6] = param.ControlOption.M2.Freq & 0xFF;
		massage.data[7] = (param.ControlOption.M2.Freq >> 8) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!VFlagNoLimit)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VFlagNoLimit;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'F';	//Flag
		massage.data[4] = 'F';	//ZeroPosition
		massage.data[5] = 0; //Write
		massage.data[6] = param.ControlOption.FlagZeroPosition;
		param.ControlOption.FlagZeroPosition = 0;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!VFlagZero)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &VFlagZero;
		*massage.flag = TRUE;

		massage.data[2] = 'M';	//Movement
		massage.data[3] = 'F';	//Flag
		massage.data[4] = 'L';	//NoLimitStepMotor
		massage.data[5] = 0; //Write
		massage.data[6] = param.ControlOption.FlagNoLimitStepMotor;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------

	ResumeThread(hTreadSendCommandProcedure);
	return TRUE;
}
//------------------------------------------------------------------------------
BOOL RequestSensorOption(void)
{
	CommandData_DType massage;
	static bool SDelay = FALSE;
	static bool SHP = FALSE;
	static bool SDamageLifetimeMin = FALSE;
	static bool SDamageLifetimeSec = FALSE;

	if (!statusConetcion)
		return FALSE;

	//--------------------------------------------------------	
	if (!SDelay)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &SDelay;
		*massage.flag = TRUE;

		massage.data[2] = 'S';	//Sensor
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'D';	//Delay
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!SHP)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &SHP;
		*massage.flag = TRUE;

		massage.data[2] = 'S';	//Sensor
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'H';	//HP
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!SDamageLifetimeMin)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &SDamageLifetimeMin;
		*massage.flag = TRUE;

		massage.data[2] = 'S';	//Sensor
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'M';	//Minutes
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!SDamageLifetimeSec)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &SDamageLifetimeSec;
		*massage.flag = TRUE;

		massage.data[2] = 'S';	//Sensor
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'S';	//Seconds
		massage.data[5] = 1; //Read

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------

	ResumeThread(hTreadSendCommandProcedure);
	return TRUE;
}
//------------------------------------------------------------------------------
BOOL SetSensorOption(void)
{
	CommandData_DType massage;
	static bool SDelay = FALSE;
	static bool SHP = FALSE;
	static bool SDamageLifetimeMin = FALSE;
	static bool SDamageLifetimeSec = FALSE;

	if (!statusConetcion)
		return FALSE;

	//--------------------------------------------------------	
	if (!SDelay)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &SDelay;
		*massage.flag = TRUE;

		massage.data[2] = 'S';	//Sensor
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'D';	//Delay
		massage.data[5] = 0; //Write
		massage.data[6] = param.DamageOption.DelaySensor_ms & 0xFF;
		massage.data[7] = (param.DamageOption.DelaySensor_ms >> 8) & 0xFF;
		massage.data[8] = (param.DamageOption.DelaySensor_ms >> 16) & 0xFF;
		massage.data[9] = (param.DamageOption.DelaySensor_ms >> 24) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!SHP)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &SHP;
		*massage.flag = TRUE;

		massage.data[2] = 'S';	//Sensor
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'H';	//HP
		massage.data[5] = 0; //Write
		massage.data[6] = param.DamageOption.HealPoint & 0xFF;
		massage.data[7] = (param.DamageOption.HealPoint >> 8) & 0xFF;
		massage.data[8] = (param.DamageOption.HealPoint >> 16) & 0xFF;
		massage.data[9] = (param.DamageOption.HealPoint >> 24) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------	
	if (!SDamageLifetimeMin)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &SDamageLifetimeMin;
		*massage.flag = TRUE;

		massage.data[2] = 'S';	//Sensor
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'M';	//Minute
		massage.data[5] = 0; //Write
		massage.data[6] = param.DamageOption.DamageDelayMinute & 0xFF;
		massage.data[7] = (param.DamageOption.DamageDelayMinute >> 8) & 0xFF;
		massage.data[8] = 0;
		massage.data[9] = 0;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------
	if (!SDamageLifetimeSec)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &SDamageLifetimeMin;
		*massage.flag = TRUE;

		massage.data[2] = 'S';	//Sensor
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'S';	//Second
		massage.data[5] = 0; //Write
		massage.data[6] = param.DamageOption.DamageDelaySecond & 0xFF;
		massage.data[7] = (param.DamageOption.DamageDelaySecond >> 8) & 0xFF;
		massage.data[8] = 0;
		massage.data[9] = 0;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	//--------------------------------------------------------

	ResumeThread(hTreadSendCommandProcedure);
	return TRUE;
}
//------------------------------------------------------------------------------