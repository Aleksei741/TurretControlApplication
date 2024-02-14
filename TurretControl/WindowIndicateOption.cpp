//******************************************************************************
//include
//******************************************************************************
#include "WindowIndicateOption.h"
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
static HWND hWndIndicateOption[10];
static CHOOSECOLOR cc;
static COLORREF dColors[] = { 255, 222, 222 };

static BOOL flagWriteGUI;
//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
LRESULT CALLBACK IndicateOptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
void WriteIndicateOptionFGUI(HWND hWnd);
//******************************************************************************
// Секция описания функций
//******************************************************************************
int WINAPI CreateWindow_IndicateOption(HINSTANCE hInst, HWND parent)
{
	hInstOption = hInst;
	flagWriteGUI = TRUE;
	WNDCLASSEX NWC = { 0 };

	//NWC.style = CS_HREDRAW | CS_HREDRAW;
	NWC.cbSize = sizeof(WNDCLASSEX);
	NWC.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	NWC.hCursor = LoadCursor(NULL, IDC_ARROW);
	NWC.hInstance = hInst;
	NWC.lpszClassName = L"IndicateOption";
	NWC.hbrBackground = (HBRUSH)COLOR_WINDOW;
	NWC.lpfnWndProc = IndicateOptionWindowProcedure;
	NWC.style = CS_HREDRAW | CS_VREDRAW;

	UnregisterClass(L"IndicateOption", hInstOption);
	if (!RegisterClassEx(&NWC)) { return -1; }

	MSG SoftwareMainMessege = { 0 };

	hwndOptionWindow = CreateWindow(L"IndicateOption", L"Настройки индикации", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 150, parent, NULL, hInst, NULL);

	ShowWindow(hwndOptionWindow, SW_NORMAL);
	UpdateWindow(hwndOptionWindow);

	cc.Flags = CC_RGBINIT | CC_FULLOPEN;
	cc.hInstance = NULL;
	cc.hwndOwner = hwndOptionWindow;
	cc.lCustData = 0L;
	cc.lpCustColors = dColors;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = (LPCWSTR)NULL;
	cc.lStructSize = sizeof(cc);
	cc.rgbResult = param.IndicateOption.Color;
	//cc.rgbResult = RGB(0, 0, 0);

	return 0;
}
//------------------------------------------------------------------------------
LRESULT CALLBACK IndicateOptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
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
			case ButtonOptionIndicateColor:
				if (ChooseColor(&cc))
				{
					param.IndicateOption.Color = (COLORREF)cc.rgbResult;
				}
				break;
			case EditOptionIndicateLineWidth:
				if (HIWORD(wp) == EN_UPDATE)
					param.IndicateOption.LineWidthPix = GetDlgItemInt(hWnd, EditOptionIndicateLineWidth, NULL, false);
				break;
			case EditOptionIndicateTextWidth:
				if (HIWORD(wp) == EN_UPDATE)
					param.IndicateOption.TextWidthPix = GetDlgItemInt(hWnd, EditOptionIndicateTextWidth, NULL, false);
				break;
			case EditOptionIndicateTextHeight:
				if (HIWORD(wp) == EN_UPDATE)
					param.IndicateOption.TextHeightPix = GetDlgItemInt(hWnd, EditOptionIndicateTextHeight, NULL, false);
				break;
			}
		}
		break;
	case WM_CREATE:	//вызывается при создании окна		
		CreateWindow(WC_STATIC, L"Параметры текста", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_INDICATE_OPTION * cnt, 180, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"ширина", WS_VISIBLE | WS_CHILD, 195, 10 + LINE_SPACE_INDICATE_OPTION * cnt, 60, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"10", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 250, 10 + LINE_SPACE_INDICATE_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionIndicateTextWidth, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"высота", WS_VISIBLE | WS_CHILD, 320, 10 + LINE_SPACE_INDICATE_OPTION * cnt, 60, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"4", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 380, 10 + LINE_SPACE_INDICATE_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionIndicateTextHeight, hInstOption, NULL);
		cnt++;

		CreateWindow(WC_STATIC, L"Толщина линий", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_INDICATE_OPTION * cnt, 300, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"23", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_INDICATE_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionIndicateLineWidth, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"pix", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_INDICATE_OPTION * cnt++, 60, 22, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Цвет линий и текста", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_INDICATE_OPTION * cnt, 180, 40, hWnd, NULL, hInstOption, NULL);
		hWndIndicateOption[AOPT_BUTTON_COLOR] = CreateWindow(WC_BUTTON, L"Выбрать", WS_CHILD | WS_VISIBLE, 350, 10 + LINE_SPACE_INDICATE_OPTION * cnt++, 80, 22, hWnd, (HMENU)ButtonOptionIndicateColor, hInstOption, NULL);
	
		WriteIndicateOptionFGUI(hWnd);
		break;
	case WM_DESTROY:	//взывается при закрытии окна		
		UnregisterClass(L"IndicateOption", hInstOption);
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
void WriteIndicateOptionFGUI(HWND hWnd)
{
	flagWriteGUI = TRUE;

	SetDlgItemInt(hWnd, EditOptionIndicateTextWidth, param.IndicateOption.TextWidthPix, false);
	SetDlgItemInt(hWnd, EditOptionIndicateTextHeight, param.IndicateOption.TextHeightPix, false);
	SetDlgItemInt(hWnd, EditOptionIndicateLineWidth, param.IndicateOption.LineWidthPix, false);

	flagWriteGUI = FALSE;
}