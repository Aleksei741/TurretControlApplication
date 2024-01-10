//******************************************************************************
//include
//******************************************************************************
#include "WindowControlOption.h"
//******************************************************************************
// Секция определения переменных, используемых в модуле
//******************************************************************************
//------------------------------------------------------------------------------
// Глобальные
//------------------------------------------------------------------------------
extern UserParameters_DType param;
//------------------------------------------------------------------------------
// Локальные
//------------------------------------------------------------------------------
static HINSTANCE hInstOption;
static HWND hwndControlOptionWindow;
static HWND hWndControlOption[10];

static BOOL flagWriteGUI;
//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
LRESULT CALLBACK ControlOptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
void WriteControlOptionFGUI(HWND hWnd);
//******************************************************************************
// Секция описания функций
//******************************************************************************
int WINAPI CreateWindow_ControlOption(HINSTANCE hInst, HWND parent)
{
	hInstOption = hInst;
	flagWriteGUI = TRUE;
	WNDCLASSEX NWC = { 0 };

	//NWC.style = CS_HREDRAW | CS_HREDRAW;
	NWC.cbSize = sizeof(WNDCLASSEX);
	NWC.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	NWC.hCursor = LoadCursor(NULL, IDC_ARROW);
	NWC.hInstance = hInst;
	NWC.lpszClassName = L"OptionControl";
	NWC.hbrBackground = (HBRUSH)COLOR_WINDOW;
	NWC.lpfnWndProc = ControlOptionWindowProcedure;
	NWC.style = CS_HREDRAW | CS_VREDRAW;

	UnregisterClass(L"OptionControl", hInstOption);
	if (!RegisterClassEx(&NWC)) { return -1; }

	MSG SoftwareMainMessege = { 0 };

	hwndControlOptionWindow = CreateWindow(L"OptionControl", L"Настройки управления", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 350, parent, NULL, hInst, NULL);

	ShowWindow(hwndControlOptionWindow, SW_NORMAL);
	UpdateWindow(hwndControlOptionWindow);

	return 0;
}
//------------------------------------------------------------------------------
LRESULT CALLBACK ControlOptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	UCHAR cnt = 0;
	LPTSTR path;
	UINT uNotify;
	int wmId;
	TCHAR szBuf[128];
	UINT value;

	switch (msg)
	{
	case WM_COMMAND:
		if (flagWriteGUI == FALSE)
		{
			switch (LOWORD(wp))
			{
			case CheckBoxM1AccelerationSpeed:
				param.ControlOption.Motor1Acceleration = SendMessage(hWndControlOption[COPT_M1ACCELERATION], BM_GETCHECK, 0, 0);
				break;
			case CheckBoxM2AccelerationSpeed:
				param.ControlOption.Motor2Acceleration = SendMessage(hWndControlOption[COPT_M2ACCELERATION], BM_GETCHECK, 0, 0);
				break;
			case EditM1SpeedLow:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM1SpeedLow, NULL, false);
					if (value < 1)
						param.ControlOption.Motor1SpeedLow = 1;
					else if (value > 1000)
						param.ControlOption.Motor1SpeedLow = 1000;
					else
						param.ControlOption.Motor1SpeedLow = value;
				}
				break;
			case EditM1SpeedHigh:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM1SpeedHigh, NULL, false);
					if (value < 1)
						param.ControlOption.Motor1SpeedHigh = 1;
					else if (value > 1000)
						param.ControlOption.Motor1SpeedHigh = 1000;
					else
						param.ControlOption.Motor1SpeedHigh = value;
				}
				break;
			case EditM2SpeedLow:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM2SpeedLow, NULL, false);
					if (value < 1)
						param.ControlOption.Motor2SpeedLow = 1;
					else if (value > 1000)
						param.ControlOption.Motor2SpeedLow = 1000;
					else
						param.ControlOption.Motor2SpeedLow = value;
				}
				break;
			case EditM2SpeedHigh:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM2SpeedHigh, NULL, false);
					if (value < 1)
						param.ControlOption.Motor2SpeedHigh = 1;
					else if (value > 1000)
						param.ControlOption.Motor2SpeedHigh = 1000;
					else
						param.ControlOption.Motor2SpeedHigh = value;
				}
				break;

			case EditM1DelayAcceleration:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM1DelayAcceleration, NULL, false);
					if (value < 500)
						param.ControlOption.Motor1DelayAcceleration = 500;
					else if (value > 60000)
						param.ControlOption.Motor1DelayAcceleration = 60000;
					else
						param.ControlOption.Motor1DelayAcceleration = value;
				}
				break;

			case EditM2DelayAcceleration:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM2DelayAcceleration, NULL, false);
					if (value < 500)
						param.ControlOption.Motor2DelayAcceleration = 500;
					else if (value > 60000)
						param.ControlOption.Motor2DelayAcceleration = 60000;
					else
						param.ControlOption.Motor2DelayAcceleration = value;
				}
				break;
				
			}
		}
		break;
	case WM_CREATE:	//вызывается при создании окна		

		CreateWindow(WC_STATIC, L"Скорость вращения 1 двигателя", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 150, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"200", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1SpeedLow, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"Гц", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);
				
		CreateWindow(WC_STATIC, L"Максимальная скорость вращения 1 двигателя", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"400", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 22, hWnd, (HMENU)EditM1SpeedHigh, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"Гц", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		hWndControlOption[COPT_M1ACCELERATION] = CreateWindow(WC_BUTTON, L"Ускорение вращения 1 двигателя", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 400, 22, hWnd, (HMENU)CheckBoxM1AccelerationSpeed, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Задержка включения ускорения 1 двигателя", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"2500", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 22, hWnd, (HMENU)EditM1DelayAcceleration, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"мс", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Скорость вращения 2 двигателя", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 150, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"200", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2SpeedLow, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"Гц", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Максимальная скорость вращения 2 двигателя", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"400", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 22, hWnd, (HMENU)EditM2SpeedHigh, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"Гц", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		hWndControlOption[COPT_M2ACCELERATION] = CreateWindow(WC_BUTTON, L"Ускорение вращения 2 двигателя", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 400, 22, hWnd, (HMENU)CheckBoxM2AccelerationSpeed, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Задержка включения ускорения 2 двигателя", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"2500", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 22, hWnd, (HMENU)EditM2DelayAcceleration, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"мс", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		WriteControlOptionFGUI(hWnd);
		break;
	case WM_DESTROY:	//взывается при закрытии окна		
		UnregisterClass(L"OptionControl", hInstOption);
		DestroyWindow(hWnd);
		return 0;
		break;

	case WM_NOTIFY:		
			break;

	case WM_PAINT:
		/*hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);*/
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}
//------------------------------------------------------------------------------
void WriteControlOptionFGUI(HWND hWnd)
{
	flagWriteGUI = TRUE;

	if (param.ControlOption.Motor1Acceleration)
		SendMessage(hWndControlOption[COPT_M1ACCELERATION], BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hWndControlOption[COPT_M1ACCELERATION], BM_SETCHECK, BST_UNCHECKED, 0);

	if (param.ControlOption.Motor2Acceleration)
		SendMessage(hWndControlOption[COPT_M2ACCELERATION], BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hWndControlOption[COPT_M2ACCELERATION], BM_SETCHECK, BST_UNCHECKED, 0);

	SetDlgItemInt(hWnd, EditM1SpeedLow, param.ControlOption.Motor1SpeedLow, false);
	SetDlgItemInt(hWnd, EditM1SpeedHigh, param.ControlOption.Motor1SpeedHigh, false);
	SetDlgItemInt(hWnd, EditM2SpeedLow, param.ControlOption.Motor2SpeedLow, false);
	SetDlgItemInt(hWnd, EditM2SpeedHigh, param.ControlOption.Motor2SpeedHigh, false);
	SetDlgItemInt(hWnd, EditM1DelayAcceleration, param.ControlOption.Motor1DelayAcceleration, false);
	SetDlgItemInt(hWnd, EditM2DelayAcceleration, param.ControlOption.Motor2DelayAcceleration, false);

	flagWriteGUI = FALSE;
}