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
static char CommandMassage[12] = { 'T', 'C', 0, 0, 0 ,0, 0, 0, 0, 0 ,0xA5 ,0xA5 };
static char ReciveMassage[12] = { 0 };
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

		//Алгоритм ускорения
		if (param.ControlOption.Motor1Acceleration &&
			MotorCommand.timeM1Action + param.ControlOption.Motor1DelayAcceleration < clock() &&
			MotorCommand.motor1 != 0 &&
			param.ControlOption.Motor1SpeedLow < param.ControlOption.Motor1SpeedHigh)
		{
			if(MotorCommand.motor1 < 0)
				MotorCommand.motor1 = -1 * param.ControlOption.Motor1SpeedHigh;
			else
				MotorCommand.motor1 = param.ControlOption.Motor1SpeedHigh;
		}

		if (param.ControlOption.Motor2Acceleration &&
			MotorCommand.timeM2Action + param.ControlOption.Motor2DelayAcceleration < clock() &&
			MotorCommand.motor2 != 0 &&
			param.ControlOption.Motor2SpeedLow < param.ControlOption.Motor2SpeedHigh)
		{
			if (MotorCommand.motor2 < 0)
				MotorCommand.motor2 = -1 * param.ControlOption.Motor2SpeedHigh;
			else
				MotorCommand.motor2 = param.ControlOption.Motor2SpeedHigh;
		}

		ResumeThread(hTreadSendCommandProcedure);
	}
}
//------------------------------------------------------------------------------
DWORD WINAPI SendCommandProcedure(CONST LPVOID lpParam)
{
	DamageStore_DType Damage;
	std::list<DamageStore_DType>::iterator itDamage;
	INT HealPoint;
	int i;

	SOCKADDR_IN addr;
	BOOL fCommand = FALSE;
	int sizeofaddr;
	static TransmiteMode_DType fLastTXMode;
	char chIPBuffer[30];
	int ret;
	clock_t paramTimeDeamage_ms;
	clock_t buffTimeDamage;

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
				fCommand = TRUE;
			}
			else
			{
				FillParamMassage();
				fCommand = FALSE;
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

			//Отображение статуса соединения 
			if (CallbackComandConectionStatus) CallbackComandConectionStatus(statusConetcion);

			//Отображение статуса работы передачи видео 
			if (CallbackVideoStatus) CallbackVideoStatus(ReciveMassage[2]);
			
			//-----------------------------------------------------------------------------------
			paramTimeDeamage_ms = param.DamageOption.DamageDelayMinute * 60 * 1000 + param.DamageOption.DamageDelaySecunde * 1000;
			//-----------------------------------------------------------------------------------
			//Подсчет HP
			if (ReciveMassage[3])
			{
				Damage.time = clock();
				Damage.val = ReciveMassage[3];

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
			//Вывод на дисплей
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
			fLastTXMode = TransmiteMode;

			//Пауза
			if (QParameters.empty() && fCommand)
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
void SetComand(BOOL fLeft, BOOL fRight, BOOL fUp, BOOL fDown, BOOL fAttack)
{
	int lr;
	int ud;

	//Определяем нажатие кнопки 
	if (fLeft)
		lr = -1 * param.ControlOption.Motor1SpeedLow;
	else if (fRight)
		lr = param.ControlOption.Motor1SpeedLow;
	else
		lr = 0;

	//Задаем движение
	if (param.HealPoint <= 0 && param.DamageOption.MovementOff)
	{
		MotorCommand.motor1 = 0;
	}
	else if (MotorCommand.motor1 == 0 && lr != 0)
	{
		MotorCommand.timeM1Action = clock();
		MotorCommand.motor1 = lr;
	}
	else if (lr == 0)
	{
		MotorCommand.motor1 = lr;
	}

	//Определяем нажатие кнопки 
	if (fUp)
		ud = param.ControlOption.Motor2SpeedLow;
	else if (fDown)
		ud = -1 * param.ControlOption.Motor2SpeedLow;
	else
		ud = 0;

	//Задаем движение
	if (param.HealPoint <= 0 && param.DamageOption.MovementOff)
	{
		MotorCommand.motor2 = 0;
	}
	else if (MotorCommand.motor2 == 0 && ud != 0)
	{
		MotorCommand.timeM2Action = clock();
		MotorCommand.motor2 = ud;
	}
	else if (ud == 0)
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
		massage.data[5] = param.DamageOption.DelaySensor_ms & 0xFF;
		massage.data[6] = (param.DamageOption.DelaySensor_ms >> 8) & 0xFF;

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