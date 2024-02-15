//******************************************************************************
//include
//******************************************************************************
#include "main.h"
#include <chrono>
#include <queue>
#include <list>
#include "CommandSend.h"
//******************************************************************************
// Секция определения переменных, используемых в модуле
//******************************************************************************
//------------------------------------------------------------------------------
// Глобальные
//------------------------------------------------------------------------------
void (*CallbackComandConectionStatus)(BOOL index);
void (*CallbackVideoStatus)(BOOL index);
void (*CallbackHPStatus)(INT index, clock_t time);
//------------------------------------------------------------------------------
// Локальные
//------------------------------------------------------------------------------
HANDLE hMutexSendCommand;
HANDLE hTreadSendCommandProcedure;
HANDLE hTreadTicProcedure;

TransmiteMode_DType TransmiteMode = TX_SOCKET_OFF;
MotorCommand_DType MotorCommand;
BOOL statusConetcion;

static std::list<DamageStore_DType> LDamage;
static BOOL ResetDamage = FALSE;

static std::queue<CommandData_DType> QParameters;
static char CommandMassage[12] = { 'T', 'C', 0, 0, 0 ,0, 0, 0, 0, 0, 0xA5, 0xA5 };
static char ReciveMassage[30] = { 0 };
SOCKET SOCKETConnection;

static char chIP[30];
//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
DWORD WINAPI SendCommandProcedure(CONST LPVOID lpParam);
DWORD WINAPI TicProcedure(CONST LPVOID lpParam);
void FillMotionCommand(void);
void FillParamMassage(void);
BOOL TurretParamSet(void);
BOOL SetControlOption(void);
void ParseInputData(void);
BOOL RequestControlOption(void);
//******************************************************************************
// Секция описания функций
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
	TransmiteMode = TX_SOCKET_OFF;
	
	if (hMutexSendCommand) CloseHandle(hMutexSendCommand);
	if (hTreadSendCommandProcedure) CloseHandle(hTreadSendCommandProcedure);
	if (hTreadTicProcedure) CloseHandle(hTreadTicProcedure);
	closesocket(SOCKETConnection);
}
//------------------------------------------------------------------------------
DWORD WINAPI TicProcedure(CONST LPVOID lpParam)
{
	while (1)
	{
		Sleep(100);
				
		//Параметры турели
		if (param.fSendTurrenParam)
		{
			if (TurretParamSet())
				param.fSendTurrenParam = FALSE;
		}

		//Заапрос параметров управления турели
		if (param.ControlOption.fSendReqParam)
		{
			if (RequestControlOption())
				param.ControlOption.fSendReqParam = FALSE;
		}

		//Уствноыка параметров управления турели
		if (param.ControlOption.fSend)
		{
			if (SetControlOption())
				param.ControlOption.fSend = FALSE;
		}

		//Запускаем передачу при работе с клавиатурой
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

		param.fSendTurrenParam = TRUE;

		while (ret == 0)
		{
			//если поменяли IP
			if (strncmp(chIP, chIPBuffer, sizeof(chIP)) != 0)
				break;

			//Выбираем команду
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
			
			//Отсылаем
			if (SOCKET_ERROR == send(SOCKETConnection, CommandMassage, sizeof(CommandMassage), NULL))
			{
				//error
				closesocket(SOCKETConnection);
				SOCKETConnection = socket(AF_INET, SOCK_STREAM, NULL);
				connect(SOCKETConnection, (SOCKADDR*)&addr, sizeof(addr));
				statusConetcion = FALSE;
				param.fSendTurrenParam = TRUE;
				memset(ReciveMassage, 0, sizeof(ReciveMassage));
			}
			else
			{
				statusConetcion = TRUE;
				recv(SOCKETConnection, ReciveMassage, sizeof(ReciveMassage), NULL);
			}

			//Обработка ReciveMassage
			ParseInputData();

			//Отображение статуса соединения 
			if (CallbackComandConectionStatus) CallbackComandConectionStatus(statusConetcion);

			//Пауза
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
	ResetDamage = TRUE;
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

	//Определяем нажатие кнопки 
	if (fLeft)
		lr = -32000;
	else if (fRight)
		lr = 32000;
	else
		lr = 0;

	//Задаем движение
	if (param.HealPoint <= 0 && param.DamageOption.MovementOff)
	{
		MotorCommand.motor1 = 0;
	}
	else
	{
		MotorCommand.motor1 = lr;
	}

	//Определяем нажатие кнопки 
	if (fUp)
		ud = -32000;
	else if (fDown)
		ud = 32000;
	else
		ud = 0;

	//Задаем движение
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

	//Задаем движение
	if (param.HealPoint <= 0 && param.DamageOption.MovementOff)
	{
		MotorCommand.motor1 = 0;
	}
	else
	{
		MotorCommand.motor1 = lr;
	}

	//Задаем движение
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
BOOL TurretParamSet(void)
{
	CommandData_DType massage;
	static bool TPDelaySensor = FALSE;

	if (!statusConetcion)
		return FALSE;

	//--------------------------------------------------------
	if (!TPDelaySensor)
	{
		massage.size = 12;
		massage.data = (unsigned char*)malloc(massage.size);
		memset(massage.data, 0, massage.size);
		massage.flag = &TPDelaySensor;
		*massage.flag = TRUE;

		massage.data[2] = 'S';	//Sensor
		massage.data[3] = 'P';	//Parameters
		massage.data[4] = 'D';	//Delay
		massage.data[5] = 0;	//Write
		massage.data[6] = param.DamageOption.DelaySensor_ms & 0xFF;
		massage.data[7] = (param.DamageOption.DelaySensor_ms >> 8) & 0xFF;

		WaitForSingleObject(hMutexSendCommand, 100);
		QParameters.push(massage);
		ReleaseMutex(hMutexSendCommand);
	}
	else
		return FALSE;
	//--------------------------------------------------------

	//ResumeThread(hTreadSendCommandProcedure);
	return TRUE;
}
//------------------------------------------------------------------------------
void ParseInputData(void)
{
	unsigned int value;
	int i;
	DamageStore_DType Damage;
	std::list<DamageStore_DType>::iterator itDamage;
	INT HealPoint;
	clock_t paramTimeDeamage_ms;
	clock_t buffTimeDamage;

	if (ReciveMassage[0] != 0x54 && ReciveMassage[1] != 0x43) //Turret Control
		return;

	//=================================================================================================================
	//рабочее сообщение	
	if (ReciveMassage[2] == 0x57) //Ответ рабочего режима
	{
		//Отображение статуса работы передачи видео 
		if (CallbackVideoStatus) CallbackVideoStatus(ReciveMassage[21]);
		//Подсчет HP
		//-----------------------------------------------------------------------------------
		paramTimeDeamage_ms = param.DamageOption.DamageDelayMinute * 60 * 1000 + param.DamageOption.DamageDelaySecunde * 1000;
		//-----------------------------------------------------------------------------------		
		if (ReciveMassage[19])
		{
			Damage.time = clock();
			Damage.val = ReciveMassage[19];

			LDamage.push_front(Damage);
		}

		if (ResetDamage)
		{
			LDamage.clear();
			ResetDamage = FALSE;
		}

		while (!LDamage.empty())
		{
			itDamage = LDamage.end();
			itDamage--;
			if (itDamage->time + paramTimeDeamage_ms < clock())
				LDamage.pop_back();
			else
				break;
		}

		HealPoint = param.DamageOption.HealPoint;
		itDamage = LDamage.begin();
		for (i = 0; i < LDamage.size(); i++)
		{
			HealPoint -= itDamage->val;
			itDamage++;
		}
		param.HealPoint = HealPoint;
		//-----------------------------------------------------------------------------------
		//Вывод HP на дисплей
		if (LDamage.empty())
		{
			buffTimeDamage = paramTimeDeamage_ms;
		}
		else
		{
			itDamage = LDamage.end();
			itDamage--;
			buffTimeDamage = itDamage->time + paramTimeDeamage_ms - clock();
		}
		if (CallbackHPStatus) CallbackHPStatus(param.HealPoint, buffTimeDamage);
		//-----------------------------------------------------------------------------------
		//Позиция турели
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
	//Параметры управления
	if (ReciveMassage[2] == 0x56) //Movement 
	{
		if (ReciveMassage[3] == 0x50) //Parameters
		{
			value = ReciveMassage[5] | (ReciveMassage[6] << 8) | (ReciveMassage[7] << 16) | (ReciveMassage[8] << 24);
			if (ReciveMassage[4] == 0x01) //MaxSteppersStepMotor1
			{
				param.ControlOption.M1.NumStepsLimit = value;
				param.ControlOption.fRecv = TRUE;
			}
			else if (ReciveMassage[4] == 0x02) //MaxSteppersStepMotor2
			{
				param.ControlOption.M2.NumStepsLimit = value;
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
}
//------------------------------------------------------------------------------
BOOL RequestControlOption(void)
{
	CommandData_DType massage;
	static bool VMaxSteppersStepM1 = FALSE;
	static bool VMaxSteppersStepM2 = FALSE;
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
		massage.data[6] = param.ControlOption.M1.NumStepsLimit & 0xFF;
		massage.data[7] = (param.ControlOption.M1.NumStepsLimit >> 8) & 0xFF;
		massage.data[8] = (param.ControlOption.M1.NumStepsLimit >> 16) & 0xFF;
		massage.data[9] = (param.ControlOption.M1.NumStepsLimit >> 24) & 0xFF;

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
		massage.data[6] = param.ControlOption.M2.NumStepsLimit & 0xFF;
		massage.data[7] = (param.ControlOption.M2.NumStepsLimit >> 8) & 0xFF;
		massage.data[8] = (param.ControlOption.M2.NumStepsLimit >> 16) & 0xFF;
		massage.data[9] = (param.ControlOption.M2.NumStepsLimit >> 24) & 0xFF;

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