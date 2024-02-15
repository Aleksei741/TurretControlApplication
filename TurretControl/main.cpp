//******************************************************************************
//include
//******************************************************************************
#include "main.h"


// Enable Visual Style
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
//******************************************************************************
// Секция определения переменных, используемых в модуле
//******************************************************************************
//------------------------------------------------------------------------------
// Глобальные
//------------------------------------------------------------------------------
UserParameters_DType param = { 0 };
//------------------------------------------------------------------------------
// Локальные
//------------------------------------------------------------------------------
HINSTANCE g_hInst;
HWND hwndMainWindow;

HWND hwndButtonConnect;
HWND hwndEditIP;
HWND hwndButtonVideo;
HWND hwndButtonResetHP;
HWND hwndEditHP;
HWND hwndEditTimer;
HWND hwndButtonMouseControl;

HWND hwndHealPointArea;
HWND hwndWideo;
HWND hwndTabc;

CHAR flagSetParamFGUI = 1;
//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure);
LRESULT CALLBACK VideoWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
void SetGUICheckBoxConectionControl(BOOL status);
void WriteParametersFGUI(void);
void StopMouseCtrl(void);
//******************************************************************************
// Секция описания функций
//******************************************************************************
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{
	setlocale(LC_ALL, "RUS");

	g_hInst = hInst;
	WNDCLASS SoftwareMainClass = NewWindowClass((HBRUSH)COLOR_WINDOW, LoadCursor(NULL, IDC_ARROW), hInst, LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)),
		L"KAMSoftware", SoftwareMainProcedure);

	if (!RegisterClass(&SoftwareMainClass)) { return -1; }
	MSG SoftwareMainMessege = { 0 };

	hwndMainWindow = CreateWindow(L"KAMSoftware", L"Turret control", WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 100, 100, 1000, 600, NULL, NULL, NULL, NULL);

	//WSAStartup
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) 
	{
		MessageBox(hwndMainWindow, L"WSAStartup не стартанула. Проблемы с DLL winsock", L"WSAData error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);
	}

	while (GetMessage(&SoftwareMainMessege, NULL, NULL, NULL))
	{
		TranslateMessage(&SoftwareMainMessege);
		DispatchMessageW(&SoftwareMainMessege);
	}

	return 0;
}
//------------------------------------------------------------------------------
WNDCLASS NewWindowClass(HBRUSH BGColor, HCURSOR Cursor, HINSTANCE hInst, HICON Icon, LPCWSTR Name, WNDPROC Procedure)
{
	WNDCLASS NWC = { 0 };

	NWC.hIcon = Icon;
	NWC.hCursor = Cursor;
	NWC.hInstance = hInst;
	NWC.lpszClassName = Name;
	NWC.hbrBackground = BGColor;
	NWC.lpfnWndProc = Procedure;

	return NWC;
}
//------------------------------------------------------------------------------
void MainWindAddMenus(HWND hWnd)
{
	HMENU RootMenu = CreateMenu();
	HMENU SubMenu = CreateMenu();
	HMENU SubMenuOption = CreateMenu();

	AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(SubMenu, MF_STRING, OnExitSoftware, L"Выход");

	AppendMenu(SubMenuOption, MF_POPUP, (UINT_PTR)OnMenuActionControlOption, L"Управление");
	AppendMenu(SubMenuOption, MF_POPUP, (UINT_PTR)OnMenuActionVideoOption, L"Видео");
	AppendMenu(SubMenuOption, MF_POPUP, (UINT_PTR)OnMenuActionDamageOption, L"Получение урона");
	AppendMenu(SubMenuOption, MF_POPUP, (UINT_PTR)OnMenuActionAimOption, L"Прицел");
	AppendMenu(SubMenuOption, MF_POPUP, (UINT_PTR)OnMenuActionIndicateOption, L"Отображение положения");
	
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"Файл");
	AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenuOption, L"Настройка");

	SetMenu(hWnd, RootMenu);
}
//------------------------------------------------------------------------------
void MainWindAddWidgets(HWND hWnd)
{
	HFONT h_font;
	h_font = CreateFont(13, 4, 0, 0,
		FW_NORMAL, 0,
		0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

	CreateWindow(WC_STATIC, L"IP", WS_VISIBLE | WS_CHILD, 5, 5, 15, 20, hWnd, NULL, g_hInst, NULL);
	hwndEditIP = CreateWindow(WC_EDIT, L"127.0.0.1", WS_VISIBLE | WS_CHILD | ES_CENTER, 20, 5, 100, 20, hWnd, (HMENU)EditIP, g_hInst, NULL);
	hwndButtonConnect = CreateWindow(WC_BUTTON, L"Connect", WS_VISIBLE | WS_CHILD, 15, 30, 100, 20, hWnd, (HMENU)ConnectButtonClik, g_hInst, NULL);
	hwndButtonVideo = CreateWindow(WC_BUTTON, L"Video: off", WS_CHILD | WS_VISIBLE, 15, 55, 100, 20, hWnd, (HMENU)VideoButtonClik, g_hInst, NULL);
	
	//hwndHealPointArea = CreateWindowEx(WS_EX_DLGMODALFRAME, WC_TREEVIEW, L"", WS_CHILD | WS_VISIBLE, 5, 85, 140, 140, hWnd, NULL, g_hInst, NULL);
	//(HBRUSH)COLOR_WINDOW
	CreateWindow(WC_EDIT, L"Heal Point", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 15, 90, 100, 20, hWnd, (HMENU)NULL, g_hInst, NULL);
	hwndEditHP = CreateWindow(WC_EDIT, L"20", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 25, 110, 25, 20, hWnd, (HMENU)EditIP, g_hInst, NULL);
	hwndEditTimer = CreateWindow(WC_EDIT, L"00:00", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 50, 110, 65, 20, hWnd, (HMENU)EditTimer, g_hInst, NULL);
	hwndButtonResetHP = CreateWindow(WC_BUTTON, L"Reset HP", WS_VISIBLE | WS_CHILD | BS_CENTER | BS_MULTILINE, 15, 130, 100, 20, hWnd, (HMENU)ResetHPButtonClik, g_hInst, NULL);
	//SendMessage(hwndButtonResetHP, WM_SETFONT, (WPARAM)h_font, (LPARAM)1);

	hwndButtonMouseControl = CreateWindow(WC_BUTTON, L"Mouse", WS_VISIBLE | WS_CHILD, 15, 160, 100, 20, hWnd, (HMENU)MouseControlButtonClik, g_hInst, NULL);
	
	SendMessage(hwndButtonConnect, BM_SETDONTCLICK, TRUE, NULL);
	SendMessage(hwndButtonVideo, BM_SETDONTCLICK, TRUE, NULL);
}
//------------------------------------------------------------------------------
HWND MainWindCreateWideoArea(HWND hWnd)
{
	RECT rcClient;
	UINT cnt;
	
	GetClientRect(hWnd, &rcClient);

	hwndWideo = CreateWindowEx(WS_EX_DLGMODALFRAME, WC_TABCONTROL, L"", WS_CHILD | WS_VISIBLE, 200, 5, rcClient.right-205, rcClient.bottom - 10, hWnd, NULL, g_hInst, NULL);

	SetWindowLongPtr(hwndWideo, GWLP_WNDPROC, (LONG_PTR)VideoWndProc);

	return hwndWideo;
}
//------------------------------------------------------------------------------
void WriteParametersFGUI(void)
{
	UINT value;
	CHAR cnt;
	TCHAR szValue[16];

	flagSetParamFGUI = 0;
	SendMessage(hwndEditIP, WM_SETTEXT, 0, (LPARAM)param.VideoOption.IP);
	flagSetParamFGUI = 1;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void WriteGUIData(void)
{

}
//------------------------------------------------------------------------------
void SetGUICheckBoxConectionControl(BOOL status)
{
	if (status)
	{
		SendMessage(hwndButtonConnect, WM_SETTEXT, 0, (LPARAM)L"Connect: on");
		Button_SetStyle(hwndButtonConnect, WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, TRUE);
	}
	else
	{
		SendMessage(hwndButtonConnect, WM_SETTEXT, 0, (LPARAM)L"Connect: off");
		Button_SetStyle(hwndButtonConnect, WS_VISIBLE | WS_CHILD, TRUE);
	}
}
//------------------------------------------------------------------------------
void SetGUIVideoStatus(BOOL status)
{
	if (status)
	{
		SendMessage(hwndButtonVideo, WM_SETTEXT, 0, (LPARAM)L"Video: on");
		Button_SetStyle(hwndButtonVideo, WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, TRUE);
		param.VideoActive = TRUE;
	}
	else
	{
		SendMessage(hwndButtonVideo, WM_SETTEXT, 0, (LPARAM)L"Video: off");
		Button_SetStyle(hwndButtonVideo, WS_VISIBLE | WS_CHILD, TRUE);
		param.VideoActive = FALSE;
	}
}
//------------------------------------------------------------------------------
void SetGUIHPStatus(INT status, clock_t time)
{
	TCHAR szBuf[10];

	static UINT lastSec = 0;
	static UINT lastMin = 0;
	UINT sec;
	UINT min;

	StringCchPrintf(szBuf, 4 / sizeof(CHAR), L"%d\0", status);
	SendMessage(hwndEditHP, WM_SETTEXT, 0, (LPARAM)szBuf);

	sec = (UINT)(time / 1000) % 60;
	min = (UINT)(time / 1000) / 60;
	if (sec != lastSec || lastMin != min)
	{
		lastSec = sec;
		lastMin = min;
		StringCchPrintf(szBuf, 10 / sizeof(CHAR), L"%d:%d\0", min, sec);
		SendMessage(hwndEditTimer, WM_SETTEXT, 0, (LPARAM)szBuf);
	}
	 
	
}
//------------------------------------------------------------------------------
void StopMouseCtrl(void)
{
	MouseProcess_stop();
	param.CotrolSource = KEYBOARD;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CALLBACK SoftwareMainProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	WCHAR szIP[30];
	HDC hDC;
	RECT Rect;

	switch (msg)
	{
	case WM_COMMAND:
		switch (wp)
		{
		case MouseControlButtonClik:
			param.CotrolSource = MOUSE;
			ESCInterrupt = &StopMouseCtrl;
			MouseProcess_start(hWnd);
			SetFocus(hWnd);
			break;
		case ResetHPButtonClik:
			DamageReset();
			SetFocus(hWnd);
			break;
		case ConnectButtonClik:
			SetIPCommand(param.VideoOption.IP);
			SetFocus(hWnd);
			break;
		case VideoButtonClik:
 			if (param.VideoActive == FALSE)
			{
				SendCommandVideoON();
			}
			else
			{
				SendCommandVideoOFF();
			}
			SetFocus(hWnd);
			break;
		case OnMenuActionVideoOption:
			CreateWindow_VideoOption(g_hInst, hwndMainWindow);
			break;
		case OnMenuActionControlOption:
			CreateWindow_ControlOption(g_hInst, hwndMainWindow);
			break;
		case OnMenuActionDamageOption:
			CreateWindow_DamageOption(g_hInst, hwndMainWindow);
			break;			
		case OnMenuActionAimOption:
			CreateWindow_AimOption(g_hInst, hwndMainWindow);
			break;
		case OnMenuActionIndicateOption:
			CreateWindow_IndicateOption(g_hInst, hwndMainWindow);
			break;
		case OnExitSoftware:
			PostQuitMessage(0);
			break;
		}

		if (LOWORD(wp) == EditIP)
		{
			if (HIWORD(wp) == EN_UPDATE)
				GetDlgItemText(hWnd, EditIP, param.VideoOption.IP, sizeof(param.VideoOption.IP) / sizeof(param.VideoOption.IP[0]));
		}
		break;
	case WM_CREATE:	//вызывается при создании окна
		memset(&param, 0, sizeof(param));
		param.CotrolSource = KEYBOARD;		

		MainWindAddMenus(hWnd);
		MainWindAddWidgets(hWnd);
		MainWindCreateWideoArea(hWnd);
		OpenFileDefault(param);
		WriteParametersFGUI();

		CallbackComandConectionStatus = &SetGUICheckBoxConectionControl;
		CallbackVideoStatus = &SetGUIVideoStatus;
		CallbackHPStatus = &SetGUIHPStatus;
		GetDlgItemText(hWnd, EditIP, szIP, sizeof(szIP) / sizeof(szIP[0]));
		SendCommandProcessStart(szIP);

		rtp_H264_recive_init();

		param.HealPoint = param.DamageOption.HealPoint;
		SetGUIHPStatus(param.HealPoint, param.DamageOption.DamageDelayMinute * 60 * 1000 + param.DamageOption.DamageDelaySecunde * 1000);

		MouseHookInterruptProcessing = &SetComandMouse;
		MouseProcess_init();

		CallbackKeyHook = &SetComandButton;
		SetKeyboardHook(NULL, NULL, NULL, NULL);
		break;
	case WM_DESTROY:	//взывается при закрытии окна
		SaveFileDefault(param);
		SendCommandProcessStop();
		DelKeyboardHook(NULL, NULL, NULL, NULL);
		MouseProcess_stop();
		MouseProcess_deinit();
		RTPStop();
		PostQuitMessage(0);
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		DeleteDC(hDC);
		break;
	case WM_NOTIFY:
		break;
	case WM_SIZE:
		GetClientRect(hWnd, &Rect);

		SetWindowPos(hwndWideo, NULL, 150, 5, Rect.right - 155, Rect.bottom - 10, NULL);
		break;
	return 0;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}
//------------------------------------------------------------------------------
LRESULT CALLBACK VideoWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	WCHAR szIP[30];
	HDC hDC;

	switch (msg)
	{	
	case WM_PAINT:
		PAINTSTRUCT ps;
		hDC = BeginPaint(hWnd, &ps);
		DrawImgCamera(hWnd, hDC);
		EndPaint(hWnd, &ps);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}
	return DefWindowProc(hWnd, msg, wp, lp);
}