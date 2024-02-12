//******************************************************************************
//include
//******************************************************************************
#include "File.h"
//******************************************************************************
// Секция определения переменных, используемых в модуле
//******************************************************************************
//------------------------------------------------------------------------------
// Глобальные
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Локальные
//------------------------------------------------------------------------------

//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
BOOL LoadParamsFFile(UserParameters_DType& param, LPWSTR Path);
void SaveParamsFFile(const UserParameters_DType param, const LPWSTR Path);
UINT Read1Param(LPWSTR szGroupName, LPWSTR szVarName, UINT DefaulValue, UCHAR paramCnt, LPWSTR Path);
void Save1Param(LPWSTR szGroupName, LPWSTR szVarName, UINT value, UCHAR paramCnt, LPWSTR Path);
//******************************************************************************
// Секция описания функций
//******************************************************************************
//Загрузка настроек по умолчанию
BOOL OpenFileDefault(UserParameters_DType& param)
{
	TCHAR szFileName[MAX_PATH], szPath[MAX_PATH];
	DWORD FileAttrib;

	// szFileName - содержит путь к exe-файлу
	// szPath - содержит путь к папке, в которой находится exe-файл	
	GetModuleFileName(0, szFileName, MAX_PATH);
	ExtractFilePath(szFileName, szPath);
	// szPath - содержит путь к файлу ini
	StringCchCat(szPath, MAX_PATH, L"setup.ini");

	/*FileAttrib = GetFileAttributes(szPath);
	if (FileAttrib == DWORD(-1))	//если не найден
		return FALSE;*/
	
	LoadParamsFFile(param, szPath);
	return TRUE;
}
//------------------------------------------------------------------------------
//Загрузка настроек по умолчанию
void SaveFileDefault(const UserParameters_DType param)
{
	TCHAR szFileName[MAX_PATH], szPath[MAX_PATH];

	// szFileName - содержит путь к exe-файлу
	// szPath - содержит путь к папке, в которой находится exe-файл	
	GetModuleFileName(0, szFileName, MAX_PATH);
	ExtractFilePath(szFileName, szPath);
	// szPath - содержит путь к файлу ini
	StringCchCat(szPath, MAX_PATH, L"setup.ini");

	SaveParamsFFile(param, szPath);
}
//------------------------------------------------------------------------------
//Загрузка пользовательских настроек
LPTSTR OpenFileUser(UserParameters_DType &param)
{
	OPENFILENAME lpofn;
	TCHAR szFile[MAX_PATH];
	CHAR openFileStatus = 0;

	ZeroMemory(&lpofn, sizeof(lpofn));
	lpofn.lStructSize = sizeof(lpofn);
	lpofn.hwndOwner = NULL;
	lpofn.lpstrFile = szFile;
	lpofn.lpstrFile[0] = '\0';
	lpofn.nMaxFile = sizeof(szFile);
	lpofn.lpstrFilter = L".ini";
	lpofn.nFilterIndex = 1;
	lpofn.lpstrFileTitle = NULL;
	lpofn.nMaxFileTitle = 0;
	lpofn.lpstrInitialDir = NULL;
	lpofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	openFileStatus = GetOpenFileName(&lpofn);

	if (openFileStatus != 0)
	{
		LoadParamsFFile(param, lpofn.lpstrFile);
	}

	return lpofn.lpstrFile;
}
//------------------------------------------------------------------------------
//Загрузка пользовательских настроек
LPTSTR SaveFileUser(const UserParameters_DType param)
{
	OPENFILENAME lpofn;
	TCHAR szFile[MAX_PATH];
	CHAR saveFileStatus = 0;	

	ZeroMemory(&lpofn, sizeof(lpofn));
	lpofn.lStructSize = sizeof(lpofn);
	lpofn.hwndOwner = NULL;
	lpofn.lpstrFile = szFile;
	lpofn.lpstrFile[0] = '\0';
	lpofn.nMaxFile = sizeof(szFile);
	lpofn.lpstrFilter = L".ini";
	lpofn.lpstrDefExt = L".ini";
	lpofn.nFilterIndex = 1;
	lpofn.lpstrFileTitle = NULL;
	lpofn.nMaxFileTitle = 0;
	lpofn.lpstrInitialDir = NULL;
	lpofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	saveFileStatus = GetSaveFileName(&lpofn);

	if (saveFileStatus != 0)
	{
		SaveParamsFFile(param, lpofn.lpstrFile);
	}

	return lpofn.lpstrFile;
}
//------------------------------------------------------------------------------
//Получение параметров из файла
BOOL LoadParamsFFile(UserParameters_DType& param, LPWSTR Path)
{
	BOOL ret = TRUE;
	CHAR cnt;
	TCHAR szBuf[128];
	UINT retParam;
	float degree_in_step;

	//-------------------------------------------------------------------------------------------------------------------------
	//VideoOption
	param.VideoOption.bitrate = Read1Param((LPWSTR)L"VideoOption", (LPWSTR)L"bitrate", 3000, 0, Path);
	param.VideoOption.framerate = Read1Param((LPWSTR)L"VideoOption", (LPWSTR)L"framerate", 25, 0, Path);
	param.VideoOption.height = Read1Param((LPWSTR)L"VideoOption", (LPWSTR)L"height", 1280, 0, Path);
	param.VideoOption.weight = Read1Param((LPWSTR)L"VideoOption", (LPWSTR)L"weight", 720, 0, Path);
	param.VideoOption.VideoPort = Read1Param((LPWSTR)L"VideoOption", (LPWSTR)L"VideoPort", 20100, 0, Path);

	//param.VideoOption.IP = Read1Param((LPWSTR)L"VideoOption", (LPWSTR)L"IP", 1, 0, Path);
	retParam = GetPrivateProfileString(
		L"VideoOption",
		L"IP",
		L"127.0.0.1",
		param.VideoOption.IP,
		30,
		Path
	);	
	//-------------------------------------------------------------------------------------------------------------------------
	//ControlOption
	//M1
	param.ControlOption.M1.RotationLimit = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_RotationLimit", 180, 0, Path);
	param.ControlOption.M1.RotationSpeedInt = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_RotationSpeedInt", 1, 0, Path);
	param.ControlOption.M1.RotationSpeedFrac = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_RotationSpeedFrac", 0, 0, Path);
	if (param.ControlOption.M1.RotationSpeedFrac < 10) param.ControlOption.M1.RotationSpeedFrac *= 100;
	else if (param.ControlOption.M1.RotationSpeedFrac < 100) param.ControlOption.M1.RotationSpeedFrac *= 10;
	param.ControlOption.M1.RotationSpeed = (float)param.ControlOption.M1.RotationSpeedInt + (float)param.ControlOption.M1.RotationSpeedFrac / 1000.0;
	param.ControlOption.M1.StepsStepperMotor = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_StepsStepperMotor", 20, 0, Path);
	param.ControlOption.M1.MicroStepsStepperMotor = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_MicroStepsStepperMotor", 64, 0, Path);
	param.ControlOption.M1.ReductionRatioStepperMotorInt = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_ReductionRatioStepperMotorInt", 10, 0, Path);
	param.ControlOption.M1.ReductionRatioStepperMotorFrac = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_ReductionRatioStepperMotorFrac", 0, 0, Path);
	if (param.ControlOption.M1.ReductionRatioStepperMotorFrac < 10) param.ControlOption.M1.ReductionRatioStepperMotorFrac *= 100;
	else if (param.ControlOption.M1.ReductionRatioStepperMotorFrac < 100) param.ControlOption.M1.ReductionRatioStepperMotorFrac *= 10;
	param.ControlOption.M1.ReductionRatioStepperMotor = (float)param.ControlOption.M1.ReductionRatioStepperMotorInt + (float)param.ControlOption.M1.ReductionRatioStepperMotorFrac / 1000.0;

	degree_in_step = 360.0 / ((float)param.ControlOption.M1.StepsStepperMotor * (float)param.ControlOption.M1.MicroStepsStepperMotor * param.ControlOption.M1.ReductionRatioStepperMotor);
	param.ControlOption.M1.Freq = (UINT)round(param.ControlOption.M1.RotationSpeed / degree_in_step);

	//M2
	param.ControlOption.M2.RotationLimit = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_RotationLimit", 180, 0, Path);
	param.ControlOption.M2.RotationSpeedInt = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_RotationSpeedInt", 1, 0, Path);
	param.ControlOption.M2.RotationSpeedFrac = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_RotationSpeedFrac", 0, 0, Path);
	if (param.ControlOption.M2.RotationSpeedFrac < 10) param.ControlOption.M2.RotationSpeedFrac *= 100;
	else if (param.ControlOption.M2.RotationSpeedFrac < 100) param.ControlOption.M2.RotationSpeedFrac *= 10;
	param.ControlOption.M2.RotationSpeed = (float)param.ControlOption.M2.RotationSpeedInt + (float)param.ControlOption.M2.RotationSpeedFrac / 1000.0;
	param.ControlOption.M2.StepsStepperMotor = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_StepsStepperMotor", 20, 0, Path);
	param.ControlOption.M2.MicroStepsStepperMotor = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_MicroStepsStepperMotor", 64, 0, Path);
	param.ControlOption.M2.ReductionRatioStepperMotorInt = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_ReductionRatioStepperMotorInt", 10, 0, Path);
	param.ControlOption.M2.ReductionRatioStepperMotorFrac = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_ReductionRatioStepperMotorFrac", 0, 0, Path);
	if (param.ControlOption.M2.ReductionRatioStepperMotorFrac < 10) param.ControlOption.M2.ReductionRatioStepperMotorFrac *= 100;
	else if (param.ControlOption.M2.ReductionRatioStepperMotorFrac < 100) param.ControlOption.M2.ReductionRatioStepperMotorFrac *= 10;
	param.ControlOption.M2.ReductionRatioStepperMotor = (float)param.ControlOption.M2.ReductionRatioStepperMotorInt + (float)param.ControlOption.M2.ReductionRatioStepperMotorFrac / 1000.0;

	degree_in_step = 360.0 / ((float)param.ControlOption.M2.StepsStepperMotor * (float)param.ControlOption.M2.MicroStepsStepperMotor * param.ControlOption.M2.ReductionRatioStepperMotor);
	param.ControlOption.M2.Freq = (UINT)round(param.ControlOption.M2.RotationSpeed / degree_in_step);
	//-------------------------------------------------------------------------------------------------------------------------
	//DamageOption
	param.DamageOption.HealPoint = Read1Param((LPWSTR)L"DamageOption", (LPWSTR)L"HealPoint", 20, 0, Path);
	param.DamageOption.MovementOff = Read1Param((LPWSTR)L"DamageOption", (LPWSTR)L"MovementOff", 1, 0, Path);
	param.DamageOption.VideoOff = Read1Param((LPWSTR)L"DamageOption", (LPWSTR)L"VideoOff", 1, 0, Path);
	param.DamageOption.WeaponOff = Read1Param((LPWSTR)L"DamageOption", (LPWSTR)L"WeaponOff", 1, 0, Path);
	param.DamageOption.DamageDelayMinute = Read1Param((LPWSTR)L"DamageOption", (LPWSTR)L"DamageDelayMinute", 2, 0, Path);
	param.DamageOption.DamageDelaySecunde = Read1Param((LPWSTR)L"DamageOption", (LPWSTR)L"DamageDelaySecunde", 30, 0, Path);
	param.DamageOption.DelaySensor_ms = Read1Param((LPWSTR)L"DamageOption", (LPWSTR)L"DelaySensor_ms", 500, 0, Path);
	//-------------------------------------------------------------------------------------------------------------------------
	//AimOption
	param.AimOption.X = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"X", 640, 0, Path);
	param.AimOption.Y = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"Y", 360, 0, Path);
	param.AimOption.height = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"height", 170, 0, Path);
	param.AimOption.width = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"width", 200, 0, Path);
	param.AimOption.LineWidthPix = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"LineWidthPix", 1, 0, Path);
	param.AimOption.TextWidthPix = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"TextWidthPix", 20, 0, Path);
	param.AimOption.TextHeightPix = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"TextHeightPix", 7, 0, Path);
	param.AimOption.TextColor = (COLORREF)Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"TextColor", 0, 0, Path);

	param.AimOption.AimText[0].Active = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[0]Active", 1, 0, Path);
	param.AimOption.AimText[0].Offset = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[0]Offset", 40, 0, Path);	
	param.AimOption.AimText[0].LenLine = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[0]LenLine", 120, 0, Path);
	retParam = GetPrivateProfileString(
		L"AimOption",
		L"AimText[0]Text",
		L"30м",
		param.AimOption.AimText[0].Text,
		40,
		Path
	);
	
	param.AimOption.AimText[1].Active = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[1]Active", 1, 0, Path);
	param.AimOption.AimText[1].Offset = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[1]Offset", 80, 0, Path);
	param.AimOption.AimText[1].LenLine = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[1]LenLine", 90, 0, Path);
	retParam = GetPrivateProfileString(
		L"AimOption",
		L"AimText[1]Text",
		L"60м",
		param.AimOption.AimText[1].Text,
		40,
		Path
	);

	param.AimOption.AimText[2].Active = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[2]Active", 1, 0, Path);
	param.AimOption.AimText[2].Offset = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[2]Offset", 120, 0, Path);
	param.AimOption.AimText[2].LenLine = Read1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[2]LenLine", 60, 0, Path);
	retParam = GetPrivateProfileString(
		L"AimOption",
		L"AimText[2]Text",
		L"100м",
		param.AimOption.AimText[2].Text,
		40,
		Path
	);
	
	//-------------------------------------------------------------------------------------------------------------------------
	return TRUE;
}
//------------------------------------------------------------------------------
//Сохранение параметров в файл
void SaveParamsFFile(const UserParameters_DType param,const LPWSTR Path)
{
	CHAR cnt;
	TCHAR szBuf[128];
	TCHAR data[128];
	UINT ret;
	//-------------------------------------------------------------------------------------------------------------------------
	//VideoOption
	Save1Param((LPWSTR)L"VideoOption", (LPWSTR)L"bitrate", param.VideoOption.bitrate, 0, Path);
	Save1Param((LPWSTR)L"VideoOption", (LPWSTR)L"framerate", param.VideoOption.framerate, 0, Path);
	Save1Param((LPWSTR)L"VideoOption", (LPWSTR)L"height", param.VideoOption.height, 0, Path);
	Save1Param((LPWSTR)L"VideoOption", (LPWSTR)L"weight", param.VideoOption.weight, 0, Path);
	Save1Param((LPWSTR)L"VideoOption", (LPWSTR)L"VideoPort", param.VideoOption.VideoPort, 0, Path);

	ret = WritePrivateProfileString(
		L"VideoOption",
		L"IP",
		param.VideoOption.IP,
		Path
	);
	//-------------------------------------------------------------------------------------------------------------------------
	//ControlOption
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_RotationLimit", param.ControlOption.M1.RotationLimit, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_RotationSpeedInt", param.ControlOption.M1.RotationSpeedInt, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_RotationSpeedFrac", param.ControlOption.M1.RotationSpeedFrac, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_StepsStepperMotor", param.ControlOption.M1.StepsStepperMotor, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_MicroStepsStepperMotor", param.ControlOption.M1.MicroStepsStepperMotor, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_ReductionRatioStepperMotorInt", param.ControlOption.M1.ReductionRatioStepperMotorInt, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M1_ReductionRatioStepperMotorFrac", param.ControlOption.M1.ReductionRatioStepperMotorFrac, 0, Path);

	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_RotationLimit", param.ControlOption.M2.RotationLimit, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_RotationSpeedInt", param.ControlOption.M2.RotationSpeedInt, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_RotationSpeedFrac", param.ControlOption.M2.RotationSpeedFrac, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_StepsStepperMotor", param.ControlOption.M2.StepsStepperMotor, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_MicroStepsStepperMotor", param.ControlOption.M2.MicroStepsStepperMotor, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_ReductionRatioStepperMotorInt", param.ControlOption.M2.ReductionRatioStepperMotorInt, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"M2_ReductionRatioStepperMotorFrac", param.ControlOption.M2.ReductionRatioStepperMotorFrac, 0, Path);
	//-------------------------------------------------------------------------------------------------------------------------
	//DamageOption
	Save1Param((LPWSTR)L"DamageOption", (LPWSTR)L"HealPoint", param.DamageOption.HealPoint, 0, Path);
	Save1Param((LPWSTR)L"DamageOption", (LPWSTR)L"MovementOff", param.DamageOption.MovementOff, 0, Path);
	Save1Param((LPWSTR)L"DamageOption", (LPWSTR)L"VideoOff", param.DamageOption.VideoOff, 0, Path);
	Save1Param((LPWSTR)L"DamageOption", (LPWSTR)L"WeaponOff", param.DamageOption.WeaponOff, 0, Path);
	Save1Param((LPWSTR)L"DamageOption", (LPWSTR)L"DamageDelayMinute", param.DamageOption.DamageDelayMinute, 0, Path);
	Save1Param((LPWSTR)L"DamageOption", (LPWSTR)L"DamageDelaySecunde", param.DamageOption.DamageDelaySecunde, 0, Path);
	Save1Param((LPWSTR)L"DamageOption", (LPWSTR)L"DelaySensor_ms", param.DamageOption.DelaySensor_ms, 0, Path);
	//-------------------------------------------------------------------------------------------------------------------------
	//AimOption
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"X", param.AimOption.X, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"Y", param.AimOption.Y, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"height", param.AimOption.height, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"width", param.AimOption.width, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"LineWidthPix", param.AimOption.LineWidthPix, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"TextWidthPix", param.AimOption.TextWidthPix, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"TextHeightPix", param.AimOption.TextHeightPix, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"TextColor", param.AimOption.TextColor, 0, Path);

	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[0]Active", param.AimOption.AimText[0].Active, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[0]Offset", param.AimOption.AimText[0].Offset, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[0]LenLine", param.AimOption.AimText[0].LenLine, 0, Path);
	ret = WritePrivateProfileString(
		L"AimOption",
		L"AimText[0]Text",
		param.AimOption.AimText[0].Text,
		Path
	);

	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[1]Active", param.AimOption.AimText[1].Active, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[1]Offset", param.AimOption.AimText[1].Offset, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[1]LenLine", param.AimOption.AimText[1].LenLine, 0, Path);
	ret = WritePrivateProfileString(
		L"AimOption",
		L"AimText[1]Text",
		param.AimOption.AimText[1].Text,
		Path
	);

	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[2]Active", param.AimOption.AimText[2].Active, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[2]Offset", param.AimOption.AimText[2].Offset, 0, Path);
	Save1Param((LPWSTR)L"AimOption", (LPWSTR)L"AimText[2]LenLine", param.AimOption.AimText[2].LenLine, 0, Path);
	ret = WritePrivateProfileString(
		L"AimOption",
		L"AimText[2]Text",
		param.AimOption.AimText[2].Text,
		Path
	);
	//-------------------------------------------------------------------------------------------------------------------------
}
//------------------------------------------------------------------------------
//Отделяет имя файла от полного пути к файлу.
LPTSTR ExtractFilePath(LPCTSTR FileName, LPTSTR buf)
{
	int i, len = lstrlen(FileName);
	for (i = len - 1; i >= 0; i--)
	{
		if (FileName[i] == '\\')
			break;
	}
	lstrcpyn(buf, FileName, i + 2);
	return buf;
}
//------------------------------------------------------------------------------
//Чтение параметра из файла
UINT Read1Param(LPWSTR szGroupName, LPWSTR szVarName, UINT DefaulValue, UCHAR paramCnt, LPWSTR Path)
{
	TCHAR szBuf[128];
	UINT result = 0;
	UINT retParam;

	if (paramCnt != 0)
		StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%s%d\0", szVarName, paramCnt);
	else
		StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%s\0", szVarName);

	retParam = GetPrivateProfileInt(
		szGroupName,
		szBuf,
		DEFAULT_VALUE_GET_FILE,
		Path
	);
	if (retParam != DEFAULT_VALUE_GET_FILE)
		result = retParam;
	else
		result = DefaulValue;

	return result;
}
//------------------------------------------------------------------------------
//Запись параметра в файл
void Save1Param(LPWSTR szGroupName, LPWSTR szVarName, UINT value, UCHAR paramCnt, LPWSTR Path)
{
	TCHAR szBuf[128];
	TCHAR data[128];

	if (paramCnt != 0)
		StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%s%d\0", szVarName, paramCnt);
	else
		StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%s\0", szVarName);

	StringCchPrintf(data, sizeof(data) / sizeof(data[0]), L"%d\0", value);
	WritePrivateProfileString(szGroupName, szBuf, data, Path);
}
//------------------------------------------------------------------------------