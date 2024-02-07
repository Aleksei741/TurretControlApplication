//******************************************************************************
//include
//******************************************************************************
#include "WindowVideoOption.h"
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
static HWND hwndOptionWindow;
static HWND hWndOption[10];

static BOOL flagWriteGUI;
//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
LRESULT CALLBACK OptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
void WriteOptionFGUI(HWND hWnd);
//******************************************************************************
// Секция описания функций
//******************************************************************************
int WINAPI CreateWindow_VideoOption(HINSTANCE hInst, HWND parent)
{
	hInstOption = hInst;
	flagWriteGUI = TRUE;
	WNDCLASSEX NWC = { 0 };

	//NWC.style = CS_HREDRAW | CS_HREDRAW;
	NWC.cbSize = sizeof(WNDCLASSEX);
	NWC.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	NWC.hCursor = LoadCursor(NULL, IDC_ARROW);
	NWC.hInstance = hInst;
	NWC.lpszClassName = L"VidepOption";
	NWC.hbrBackground = (HBRUSH)COLOR_WINDOW;
	NWC.lpfnWndProc = OptionWindowProcedure;
	NWC.style = CS_HREDRAW | CS_VREDRAW;

	UnregisterClass(L"VidepOption", hInstOption);
	if (!RegisterClassEx(&NWC)) { return -1; }

	MSG SoftwareMainMessege = { 0 };

	hwndOptionWindow = CreateWindow(L"VidepOption", L"Настройки видео", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 250, parent, NULL, hInst, NULL);

	ShowWindow(hwndOptionWindow, SW_NORMAL);
	UpdateWindow(hwndOptionWindow);

	return 0;
}
//------------------------------------------------------------------------------
LRESULT CALLBACK OptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	UCHAR cnt = 0;
	LPTSTR path;
	UINT uNotify;
	int wmId;
	TCHAR szBuf[128];
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
	case WM_COMMAND:
		if (flagWriteGUI == FALSE)
		{
			switch (LOWORD(wp))
			{
			/*case CheckBoxOptionWindowAction:
				param.VideoOption.WindowAction = SendMessage(hWndOption[HOPT_WINDOW_ACTION], BM_GETCHECK, 0, 0);
				break;*/
			case EditOptionHeight:
				if (HIWORD(wp) == EN_UPDATE)
				{
					param.VideoOption.height = GetDlgItemInt(hWnd, EditOptionHeight, NULL, false);
					param.AimOption.X = param.VideoOption.height / 2;
				}
				break;
			case EditOptionWeight:
				if (HIWORD(wp) == EN_UPDATE)
				{
					param.VideoOption.weight = GetDlgItemInt(hWnd, EditOptionWeight, NULL, false);
					param.AimOption.Y = param.VideoOption.weight / 2;
				}
				break;
			case EditOptionFramerate:
				if (HIWORD(wp) == EN_UPDATE)
					param.VideoOption.framerate = GetDlgItemInt(hWnd, EditOptionFramerate, NULL, false);
				break;
			case EditOptionBitrate:
				if (HIWORD(wp) == EN_UPDATE)
					param.VideoOption.bitrate = GetDlgItemInt(hWnd, EditOptionBitrate, NULL, false);
				break;
			case EditOptionVideoPort:
				param.VideoOption.VideoPort = GetDlgItemInt(hWnd, EditOptionVideoPort, NULL, false);
				break;			
			}
		}
		break;
	case WM_CREATE:	//вызывается при создании окна		
		CreateWindow(WC_STATIC, L"Ширина изображения", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_OPTION * cnt, 150, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"1080", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_OPTION * cnt, 60, 25, hWnd, (HMENU)EditOptionHeight, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"pix", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);
				
		CreateWindow(WC_STATIC, L"Высота изображения", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"720", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionWeight, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"pix", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Кадров в секунду", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"25", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionFramerate, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"кадр/сек", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Bitrate", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"4000", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionBitrate, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"K", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Порт для видео", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"20100", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionVideoPort, hInstOption, NULL);
		CreateWindow(WC_STATIC, L" - ", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);
	
		WriteOptionFGUI(hWnd);
		break;
	case WM_DESTROY:	//взывается при закрытии окна		
		UnregisterClass(L"VidepOption", hInstOption);
		DestroyWindow(hWnd);
		return 0;
		break;

	case WM_NOTIFY:		
			break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	default: return DefWindowProc(hWnd, msg, wp, lp);
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}
//------------------------------------------------------------------------------
void WriteOptionFGUI(HWND hWnd)
{
	flagWriteGUI = TRUE;

	SetDlgItemInt(hWnd, EditOptionHeight, param.VideoOption.height, false);
	SetDlgItemInt(hWnd, EditOptionWeight, param.VideoOption.weight, false);
	SetDlgItemInt(hWnd, EditOptionFramerate, param.VideoOption.framerate, false);
	SetDlgItemInt(hWnd, EditOptionBitrate, param.VideoOption.bitrate, false);
	SetDlgItemInt(hWnd, EditOptionVideoPort, param.VideoOption.VideoPort, false);

	flagWriteGUI = FALSE;
}