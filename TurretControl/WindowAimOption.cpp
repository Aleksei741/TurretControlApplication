//******************************************************************************
//include
//******************************************************************************
#include "WindowAimOption.h"
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
static HWND hWndAimOption[10];
static CHOOSECOLOR cc;
static COLORREF dColors[] = { 255, 222, 222 };

static BOOL flagWriteGUI;
//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
LRESULT CALLBACK AimOptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
void WriteAimOptionFGUI(HWND hWnd);
//******************************************************************************
// Секция описания функций
//******************************************************************************
int WINAPI CreateWindow_AimOption(HINSTANCE hInst, HWND parent)
{
	hInstOption = hInst;
	flagWriteGUI = TRUE;
	WNDCLASSEX NWC = { 0 };

	//NWC.style = CS_HREDRAW | CS_HREDRAW;
	NWC.cbSize = sizeof(WNDCLASSEX);
	NWC.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	NWC.hCursor = LoadCursor(NULL, IDC_ARROW);
	NWC.hInstance = hInst;
	NWC.lpszClassName = L"AimOption";
	NWC.hbrBackground = (HBRUSH)COLOR_WINDOW;
	NWC.lpfnWndProc = AimOptionWindowProcedure;
	NWC.style = CS_HREDRAW | CS_VREDRAW;

	UnregisterClass(L"AimOption", hInstOption);
	if (!RegisterClassEx(&NWC)) { return -1; }

	MSG SoftwareMainMessege = { 0 };

	hwndOptionWindow = CreateWindow(L"AimOption", L"Настройки прицела", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 480, 320, parent, NULL, hInst, NULL);

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
	cc.rgbResult = param.AimOption.TextColor;
	//cc.rgbResult = RGB(0, 0, 0);

	return 0;
}
//------------------------------------------------------------------------------
LRESULT CALLBACK AimOptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
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
			case CheckBoxText1:
				param.AimOption.AimText[0].Active = SendMessage(hWndAimOption[AOPT_MEAS_1], BM_GETCHECK, 0, 0);
				break;
			case CheckBoxText2:
				param.AimOption.AimText[1].Active = SendMessage(hWndAimOption[AOPT_MEAS_2], BM_GETCHECK, 0, 0);
				break;
			case CheckBoxText3:
				param.AimOption.AimText[2].Active = SendMessage(hWndAimOption[AOPT_MEAS_3], BM_GETCHECK, 0, 0);
				break;
			case ButtonOptionAimColorText:
				if (ChooseColor(&cc)) 
				{
					param.AimOption.TextColor = (COLORREF)cc.rgbResult;
				}				
				break;
			case EditOptionAimX:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.X = GetDlgItemInt(hWnd, EditOptionAimX, NULL, false);
				break;
			case EditOptionAimY:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.Y = GetDlgItemInt(hWnd, EditOptionAimY, NULL, false);
				break;
			case EditOptionAimHeight:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.height = GetDlgItemInt(hWnd, EditOptionAimHeight, NULL, false);
				break;
			case EditOptionAimWidth:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.width = GetDlgItemInt(hWnd, EditOptionAimWidth, NULL, false);
				break;
			case EditOptionAimLineWidth:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.LineWidthPix = GetDlgItemInt(hWnd, EditOptionAimLineWidth, NULL, false);
				break;	
			case EditOptionAimTextWidth:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.TextWidthPix = GetDlgItemInt(hWnd, EditOptionAimTextWidth, NULL, false);
				break;
			case EditOptionAimTextHeight:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.TextHeightPix = GetDlgItemInt(hWnd, EditOptionAimTextHeight, NULL, false);
				break;
			case EditOptionLenLineText1:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.AimText[0].LenLine = GetDlgItemInt(hWnd, EditOptionLenLineText1, NULL, false);
				break;
			case EditOptionOffsetText1:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.AimText[0].Offset = GetDlgItemInt(hWnd, EditOptionOffsetText1, NULL, false);
				break;
			case EditOptionText1:
				if (HIWORD(wp) == EN_UPDATE)
					GetDlgItemText(hWnd, EditOptionText1, param.AimOption.AimText[0].Text, 40);
				break;
			case EditOptionLenLineText2:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.AimText[1].LenLine = GetDlgItemInt(hWnd, EditOptionLenLineText2, NULL, false);
				break;
			case EditOptionOffsetText2:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.AimText[1].Offset = GetDlgItemInt(hWnd, EditOptionOffsetText2, NULL, false);
				break;
			case EditOptionText2:
				if (HIWORD(wp) == EN_UPDATE)
					GetDlgItemText(hWnd, EditOptionText2, param.AimOption.AimText[1].Text, 40);
				break;
			case EditOptionLenLineText3:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.AimText[2].LenLine = GetDlgItemInt(hWnd, EditOptionLenLineText3, NULL, false);
				break;
			case EditOptionOffsetText3:
				if (HIWORD(wp) == EN_UPDATE)
					param.AimOption.AimText[2].Offset = GetDlgItemInt(hWnd, EditOptionOffsetText3, NULL, false);
				break;
			case EditOptionText3:
				if (HIWORD(wp) == EN_UPDATE)
					GetDlgItemText(hWnd, EditOptionText3, param.AimOption.AimText[2].Text, 40);
				break;
			}
		}
		break;
	case WM_CREATE:	//вызывается при создании окна		
		CreateWindow(WC_STATIC, L"Координаты прицела X", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_AIM_OPTION * cnt, 300, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"100", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_AIM_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionAimX, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"pix", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_AIM_OPTION * cnt++, 60, 22, hWnd, NULL, hInstOption, NULL);
				
		CreateWindow(WC_STATIC, L"Координаты прицела Y", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_AIM_OPTION * cnt, 300, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"100", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_AIM_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionAimY, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"pix", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_AIM_OPTION * cnt++, 60, 22, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Высота прицела", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_AIM_OPTION * cnt, 300, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"25", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_AIM_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionAimHeight, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"pix", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_AIM_OPTION * cnt++, 60, 22, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Ширина прицела", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_AIM_OPTION * cnt, 300, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"23", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_AIM_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionAimWidth, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"pix", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_AIM_OPTION * cnt++, 60, 22, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Толщина линий", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_AIM_OPTION * cnt, 300, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"23", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_AIM_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionAimLineWidth, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"pix", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_AIM_OPTION * cnt++, 60, 22, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Параметры текста", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_AIM_OPTION * cnt, 180, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"высота", WS_VISIBLE | WS_CHILD, 195, 10 + LINE_SPACE_AIM_OPTION * cnt, 60, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"10", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 250, 10 + LINE_SPACE_AIM_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionAimTextWidth, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"ширина", WS_VISIBLE | WS_CHILD, 320, 10 + LINE_SPACE_AIM_OPTION * cnt, 60, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"4", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 380, 10 + LINE_SPACE_AIM_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionAimTextHeight, hInstOption, NULL);
		cnt++;

		CreateWindow(WC_STATIC, L"Цвет линий и текста", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_AIM_OPTION * cnt, 180, 40, hWnd, NULL, hInstOption, NULL);
		hWndAimOption[AOPT_BUTTON_COLOR] = CreateWindow(WC_BUTTON, L"Выбрать", WS_CHILD | WS_VISIBLE, 350, 10 + LINE_SPACE_AIM_OPTION * cnt++, 80, 22, hWnd, (HMENU)ButtonOptionAimColorText, hInstOption, NULL);

		hWndAimOption[AOPT_MEAS_1] = CreateWindow(WC_BUTTON, L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10 + LINE_SPACE_AIM_OPTION * cnt, 20, 22, hWnd, (HMENU)CheckBoxText1, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"длина линии", WS_VISIBLE | WS_CHILD, 40, 10 + LINE_SPACE_AIM_OPTION * cnt, 45, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"20", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 90, 10 + LINE_SPACE_AIM_OPTION * cnt, 30, 22, hWnd, (HMENU)EditOptionLenLineText1, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"смещение", WS_VISIBLE | WS_CHILD, 140, 10 + LINE_SPACE_AIM_OPTION * cnt, 70, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"45", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 215, 10 + LINE_SPACE_AIM_OPTION * cnt, 30, 22, hWnd, (HMENU)EditOptionOffsetText1, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"текст", WS_VISIBLE | WS_CHILD, 260, 10 + LINE_SPACE_AIM_OPTION * cnt, 45, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"30м", WS_VISIBLE | WS_CHILD | ES_CENTER, 305, 10 + LINE_SPACE_AIM_OPTION * cnt++, 120, 22, hWnd, (HMENU)EditOptionText1, hInstOption, NULL);

		hWndAimOption[AOPT_MEAS_2] = CreateWindow(WC_BUTTON, L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10 + LINE_SPACE_AIM_OPTION * cnt, 20, 22, hWnd, (HMENU)CheckBoxText2, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"длина линии", WS_VISIBLE | WS_CHILD, 40, 10 + LINE_SPACE_AIM_OPTION * cnt, 45, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"20", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 90, 10 + LINE_SPACE_AIM_OPTION * cnt, 30, 22, hWnd, (HMENU)EditOptionLenLineText2, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"смещение", WS_VISIBLE | WS_CHILD, 140, 10 + LINE_SPACE_AIM_OPTION * cnt, 70, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"45", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 215, 10 + LINE_SPACE_AIM_OPTION * cnt, 30, 22, hWnd, (HMENU)EditOptionOffsetText2, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"текст", WS_VISIBLE | WS_CHILD, 260, 10 + LINE_SPACE_AIM_OPTION * cnt, 45, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"30м", WS_VISIBLE | WS_CHILD | ES_CENTER, 305, 10 + LINE_SPACE_AIM_OPTION * cnt++, 120, 22, hWnd, (HMENU)EditOptionText2, hInstOption, NULL);

		hWndAimOption[AOPT_MEAS_3] = CreateWindow(WC_BUTTON, L"", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10 + LINE_SPACE_AIM_OPTION * cnt, 20, 22, hWnd, (HMENU)CheckBoxText3, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"длина линии", WS_VISIBLE | WS_CHILD, 40, 10 + LINE_SPACE_AIM_OPTION * cnt, 45, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"20", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 90, 10 + LINE_SPACE_AIM_OPTION * cnt, 30, 22, hWnd, (HMENU)EditOptionLenLineText3, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"смещение", WS_VISIBLE | WS_CHILD, 140, 10 + LINE_SPACE_AIM_OPTION * cnt, 70, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"45", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 215, 10 + LINE_SPACE_AIM_OPTION * cnt, 30, 22, hWnd, (HMENU)EditOptionOffsetText3, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"текст", WS_VISIBLE | WS_CHILD, 260, 10 + LINE_SPACE_AIM_OPTION * cnt, 45, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"30м", WS_VISIBLE | WS_CHILD | ES_CENTER, 305, 10 + LINE_SPACE_AIM_OPTION * cnt++, 120, 22, hWnd, (HMENU)EditOptionText3, hInstOption, NULL);
		WriteAimOptionFGUI(hWnd);
		break;
	case WM_DESTROY:	//взывается при закрытии окна		
		UnregisterClass(L"AimOption", hInstOption);
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
void WriteAimOptionFGUI(HWND hWnd)
{
	flagWriteGUI = TRUE;

	if (param.AimOption.AimText[0].Active)
		SendMessage(hWndAimOption[AOPT_MEAS_1], BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hWndAimOption[AOPT_MEAS_1], BM_SETCHECK, BST_UNCHECKED, 0);

	if (param.AimOption.AimText[1].Active)
		SendMessage(hWndAimOption[AOPT_MEAS_2], BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hWndAimOption[AOPT_MEAS_2], BM_SETCHECK, BST_UNCHECKED, 0);

	if (param.AimOption.AimText[2].Active)
		SendMessage(hWndAimOption[AOPT_MEAS_3], BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hWndAimOption[AOPT_MEAS_3], BM_SETCHECK, BST_UNCHECKED, 0);

	SetDlgItemInt(hWnd, EditOptionAimX, param.AimOption.X, false);
	SetDlgItemInt(hWnd, EditOptionAimY, param.AimOption.Y, false);
	SetDlgItemInt(hWnd, EditOptionAimHeight, param.AimOption.height, false);
	SetDlgItemInt(hWnd, EditOptionAimWidth, param.AimOption.width, false);
	SetDlgItemInt(hWnd, EditOptionAimLineWidth, param.AimOption.LineWidthPix, false);
	SetDlgItemInt(hWnd, EditOptionAimTextWidth, param.AimOption.TextWidthPix, false);
	SetDlgItemInt(hWnd, EditOptionAimTextHeight, param.AimOption.TextHeightPix, false);

	SetDlgItemInt(hWnd, EditOptionLenLineText1, param.AimOption.AimText[0].LenLine, false);
	SetDlgItemInt(hWnd, EditOptionOffsetText1, param.AimOption.AimText[0].Offset, false);
	SetDlgItemText(hWnd, EditOptionText1, param.AimOption.AimText[0].Text);

	SetDlgItemInt(hWnd, EditOptionLenLineText2, param.AimOption.AimText[1].LenLine, false);
	SetDlgItemInt(hWnd, EditOptionOffsetText2, param.AimOption.AimText[1].Offset, false);
	SetDlgItemText(hWnd, EditOptionText2, param.AimOption.AimText[1].Text);

	SetDlgItemInt(hWnd, EditOptionLenLineText3, param.AimOption.AimText[2].LenLine, false);
	SetDlgItemInt(hWnd, EditOptionOffsetText3, param.AimOption.AimText[2].Offset, false);
	SetDlgItemText(hWnd, EditOptionText3, param.AimOption.AimText[2].Text);

	flagWriteGUI = FALSE;
}