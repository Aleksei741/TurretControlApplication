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
	param.ControlOption.Motor1Acceleration = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor1Acceleration", 1, 0, Path);
	param.ControlOption.Motor1SpeedHigh = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor1SpeedHigh", 400, 0, Path);
	param.ControlOption.Motor1SpeedLow = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor1SpeedLow", 200, 0, Path);
	param.ControlOption.Motor2Acceleration = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor2Acceleration", 1, 0, Path);
	param.ControlOption.Motor2SpeedHigh = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor2SpeedHigh", 400, 0, Path);
	param.ControlOption.Motor2SpeedLow = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor2SpeedLow", 200, 0, Path);
	param.ControlOption.Motor1DelayAcceleration = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor1DelayAcceleration", 2500, 0, Path);
	param.ControlOption.Motor2DelayAcceleration = Read1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor2DelayAcceleration", 2500, 0, Path);
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
	//VideoOption
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor1Acceleration", param.ControlOption.Motor1Acceleration, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor1SpeedHigh", param.ControlOption.Motor1SpeedHigh, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor1SpeedLow", param.ControlOption.Motor1SpeedLow, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor2Acceleration", param.ControlOption.Motor2Acceleration, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor2SpeedHigh", param.ControlOption.Motor2SpeedHigh, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor2SpeedLow", param.ControlOption.Motor2SpeedLow, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor1DelayAcceleration", param.ControlOption.Motor1DelayAcceleration, 0, Path);
	Save1Param((LPWSTR)L"ControlOption", (LPWSTR)L"Motor2DelayAcceleration", param.ControlOption.Motor2DelayAcceleration, 0, Path);
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