//******************************************************************************
//include
//******************************************************************************
#include "WindowControlOption.h"
#include <math.h> 
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

static WNDPROC OriginalEditM1RotationSpeed;
static WNDPROC OriginalEditM2RotationSpeed;
static WNDPROC OriginalEditM1ReductionRatio;
static WNDPROC OriginalEditM2ReductionRatio;
static WNDPROC OriginalEditMouseCoefficient;
//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
LRESULT CALLBACK ControlOptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
void WriteControlOptionFGUI(HWND hWnd);
void WriteControlCalcParamFGUI(HWND hWnd);
void ReverseCalcParamFGUI(void);
LRESULT CALLBACK EditM1RotationSpeedWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK EditM2RotationSpeedWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK EditM1ReductionRatioWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK EditM2ReductionRatioWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
LRESULT CALLBACK MouseCoefficientWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
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

	hwndControlOptionWindow = CreateWindow(L"OptionControl", L"Настройки управления", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 750, parent, NULL, hInst, NULL);

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
	char chBuf[128];
	UINT value;
	size_t nNumCharConverted;
	float fractpart = 0;
	double atofBuf;
	static SCROLLINFO lpsi = { 0 };

	switch (msg)
	{
	case WM_COMMAND:
		if (flagWriteGUI == FALSE)
		{
			switch (LOWORD(wp))
			{
			//M1
			case EditM1MaxDegreePosition:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM1MaxDegreePosition, NULL, false);
					if (value >= 180)
						param.ControlOption.M1.MaxDegreePosition = 180;
					else
						param.ControlOption.M1.MaxDegreePosition = value;

					WriteControlCalcParamFGUI(hWnd);
				}
				break;			
			case EditM1MinDegreePosition:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM1MinDegreePosition, NULL, false);
					if (value >= 180)
						param.ControlOption.M1.MinDegreePosition = 180;
					else
						param.ControlOption.M1.MinDegreePosition = value;

					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM1RotationSpeed:
				if (HIWORD(wp) == EN_UPDATE)
				{
					GetDlgItemText(hWnd, EditM1RotationSpeed, szBuf, sizeof(szBuf) / sizeof(szBuf[0]));
					wcstombs_s(&nNumCharConverted, chBuf, sizeof(chBuf) / sizeof(chBuf[0]), szBuf, sizeof(szBuf) / sizeof(szBuf[0]));
					atofBuf = atof(chBuf);
					param.ControlOption.M1.RotationSpeed = (float)atofBuf;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;

			case EditM1MicroStepsStepperMotor:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM1MicroStepsStepperMotor, NULL, false);
					if (value < 1)
						param.ControlOption.M1.MicroStepsStepperMotor = 1;
					else if (value > 60000)
						param.ControlOption.M1.MicroStepsStepperMotor = 60000;
					else
						param.ControlOption.M1.MicroStepsStepperMotor = value;

					WriteControlCalcParamFGUI(hWnd);
				}
				break;

			case EditM1ReductionRatioStepperMotor:
				if (HIWORD(wp) == EN_UPDATE)
				{
					GetDlgItemText(hWnd, EditM1ReductionRatioStepperMotor, szBuf, sizeof(szBuf) / sizeof(szBuf[0]));
					wcstombs_s(&nNumCharConverted, chBuf, sizeof(chBuf) / sizeof(chBuf[0]), szBuf, sizeof(szBuf) / sizeof(szBuf[0]));
					atofBuf = atof(chBuf);
					param.ControlOption.M1.ReductionRatioStepperMotor = (float)atofBuf;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			//M2
			case EditM2MaxDegreePosition:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM2MaxDegreePosition, NULL, false);
					if (value >= 180)
						param.ControlOption.M2.MaxDegreePosition = 180;
					else
						param.ControlOption.M2.MaxDegreePosition = value;

					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM2MinDegreePosition:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM2MinDegreePosition, NULL, false);
					if (value >= 180)
						param.ControlOption.M2.MinDegreePosition = 180;
					else
						param.ControlOption.M2.MinDegreePosition = value;

					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM2RotationSpeed:
				if (HIWORD(wp) == EN_UPDATE)
				{
					GetDlgItemText(hWnd, EditM2RotationSpeed, szBuf, sizeof(szBuf) / sizeof(szBuf[0]));
					wcstombs_s(&nNumCharConverted, chBuf, sizeof(chBuf) / sizeof(chBuf[0]), szBuf, sizeof(szBuf) / sizeof(szBuf[0]));
					atofBuf = atof(chBuf);
					param.ControlOption.M2.RotationSpeed = (float)atofBuf;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;

			case EditM2MicroStepsStepperMotor:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM2MicroStepsStepperMotor, NULL, false);
					if (value < 1)
						param.ControlOption.M2.MicroStepsStepperMotor = 1;
					else if (value > 60000)
						param.ControlOption.M2.MicroStepsStepperMotor = 60000;
					else
						param.ControlOption.M2.MicroStepsStepperMotor = value;

					WriteControlCalcParamFGUI(hWnd);
				}
				break;

			case EditM2ReductionRatioStepperMotor:
				if (HIWORD(wp) == EN_UPDATE)
				{
					GetDlgItemText(hWnd, EditM2ReductionRatioStepperMotor, szBuf, sizeof(szBuf) / sizeof(szBuf[0]));
					wcstombs_s(&nNumCharConverted, chBuf, sizeof(chBuf) / sizeof(chBuf[0]), szBuf, sizeof(szBuf) / sizeof(szBuf[0]));
					atofBuf = atof(chBuf);
					param.ControlOption.M2.ReductionRatioStepperMotor = (float)atofBuf;
					WriteControlCalcParamFGUI(hWnd);
				}			
				break;
			case CheckBoxNoLimit:
				param.ControlOption.FlagNoLimitStepMotor = SendMessage(hWndControlOption[COPT_NO_LIMIT], BM_GETCHECK, 0, 0);
				param.ControlOption.fSend = TRUE; //Посылаем параметры в турель
				break;
			case ButtonOptionZeroPosition:
				param.ControlOption.FlagZeroPosition = TRUE;
				param.ControlOption.fSend = TRUE; //Посылаем параметры в турель
				break;
			}
		}
		break;
	case WM_CREATE:	//вызывается при создании окна	
		param.ControlOption.fSendReqParam = TRUE; //Запрос параметров

		CreateWindow(WC_STATIC, L"1 двигатель", WS_VISIBLE | WS_CHILD, 190, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 150, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Предел вращения вправо", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"90", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1MaxDegreePosition, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"град", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Предел вращения влево", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"90", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1MinDegreePosition, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"град", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Скорость вращения", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 200, 25, hWnd, NULL, hInstOption, NULL);
		hWndControlOption[COPT_EDIT_M1_ROTATION_SPEED] = CreateWindow(WC_EDIT, L"1", WS_VISIBLE | WS_CHILD | ES_CENTER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1RotationSpeed, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"град/сек", WS_VISIBLE | WS_CHILD, 416, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);
		OriginalEditM1RotationSpeed = (WNDPROC) SetWindowLongPtr(hWndControlOption[COPT_EDIT_M1_ROTATION_SPEED], GWLP_WNDPROC, (LONG_PTR)EditM1RotationSpeedWndProc);

		CreateWindow(WC_STATIC, L"Микрошагов на один оборот двигателя", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 150, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"64", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1MicroStepsStepperMotor, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"мик.шаг.", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Коэффициент редукции редуктора", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		hWndControlOption[COPT_EDIT_M1_REDUCTION_RATIO] = CreateWindow(WC_EDIT, L"1", WS_VISIBLE | WS_CHILD | ES_CENTER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1ReductionRatioStepperMotor, hInstOption, NULL);
		CreateWindow(WC_STATIC, L" ", WS_VISIBLE | WS_CHILD, 416, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);
		OriginalEditM1ReductionRatio = (WNDPROC)SetWindowLongPtr(hWndControlOption[COPT_EDIT_M1_REDUCTION_RATIO], GWLP_WNDPROC, (LONG_PTR)EditM1ReductionRatioWndProc);

		CreateWindow(WC_STATIC, L"Частота сигнала", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1FreqSignal, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"Гц", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Период сигнала", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1PeriodSignal, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"с", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		cnt++;

		CreateWindow(WC_STATIC, L"2 двигатель", WS_VISIBLE | WS_CHILD, 190, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 150, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Предел вращения вверх", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"90", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2MaxDegreePosition, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"град", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Предел вращения вниз", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"90", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2MinDegreePosition, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"град", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Скорость вращения", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 200, 25, hWnd, NULL, hInstOption, NULL);
		hWndControlOption[COPT_EDIT_M2_ROTATION_SPEED] = CreateWindow(WC_EDIT, L"1", WS_VISIBLE | WS_CHILD | ES_CENTER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2RotationSpeed, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"град/сек", WS_VISIBLE | WS_CHILD, 416, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);
		OriginalEditM2RotationSpeed = (WNDPROC)SetWindowLongPtr(hWndControlOption[COPT_EDIT_M2_ROTATION_SPEED], GWLP_WNDPROC, (LONG_PTR)EditM2RotationSpeedWndProc);

		CreateWindow(WC_STATIC, L"Микрошагов на один оборот двигателя", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 150, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"64", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2MicroStepsStepperMotor, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"мик.шаг.", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Коэффициент редукции редуктора", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		hWndControlOption[COPT_EDIT_M2_REDUCTION_RATIO] = CreateWindow(WC_EDIT, L"1", WS_VISIBLE | WS_CHILD | ES_CENTER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2ReductionRatioStepperMotor, hInstOption, NULL);
		CreateWindow(WC_STATIC, L" ", WS_VISIBLE | WS_CHILD, 416, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);
		OriginalEditM1ReductionRatio = (WNDPROC)SetWindowLongPtr(hWndControlOption[COPT_EDIT_M2_REDUCTION_RATIO], GWLP_WNDPROC, (LONG_PTR)EditM2ReductionRatioWndProc);

		CreateWindow(WC_STATIC, L"Частота сигнала", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2FreqSignal, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"Гц", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Период сигнала", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2PeriodSignal, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"с", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		cnt++;
		CreateWindow(WC_STATIC, L"Обнулить координаты положения турели", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		hWndControlOption[COPT_ZERO_POSITION] = CreateWindow(WC_BUTTON, L"-", WS_CHILD | WS_VISIBLE, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 22, hWnd, (HMENU)ButtonOptionZeroPosition, hInstOption, NULL);

		hWndControlOption[COPT_NO_LIMIT] = CreateWindow(WC_BUTTON, L"Не ограничивать движение туели", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 300, 22, hWnd, (HMENU)CheckBoxNoLimit, hInstOption, NULL);

		cnt++;
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 300, 10 + LINE_SPACE_CONTROL_OPTION * cnt - 20, 60, 20, hWnd, (HMENU)EditMouseCoefficient, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"Чувствительность мыши", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 150, 40, hWnd, NULL, hInstOption, NULL);
		hWndControlOption[COPT_MOUSE_COEFFICIENT] = CreateWindow(WC_SCROLLBAR, NULL, WS_CHILD | WS_VISIBLE | SBS_HORZ, 160, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 320, 25, hWnd, (HMENU)ScrollbarMouseCoefficient, hInstOption, NULL);
		
		lpsi.nMin = 1;
		lpsi.nMax = 250;
		lpsi.cbSize = sizeof(lpsi);
		lpsi.fMask = SIF_RANGE | SIF_POS;
		lpsi.nPos = (param.ControlOption.MouseCoefficient * 100);

		SetScrollInfo(hWndControlOption[COPT_MOUSE_COEFFICIENT], SB_CTL, &lpsi, TRUE );

		WriteControlOptionFGUI(hWnd);

		SetTimer(hWnd, COPT_TIMER, 1000, NULL);
		break;
	case WM_HSCROLL:
		switch (LOWORD(wp))
		{
		case SB_LINELEFT:
			if(param.ControlOption.MouseCoefficient > 0.01)
				param.ControlOption.MouseCoefficient -= 0.01;
			break;
		case SB_PAGELEFT:
			if (param.ControlOption.MouseCoefficient > 0.1)
				param.ControlOption.MouseCoefficient -= 0.1;
			break;
		case SB_LINERIGHT:
			if (param.ControlOption.MouseCoefficient < 2.50)
				param.ControlOption.MouseCoefficient += 0.01;
			break;
		case SB_PAGERIGHT:
			if (param.ControlOption.MouseCoefficient < 2.40)
				param.ControlOption.MouseCoefficient += 0.1;
			break;
		case SB_THUMBPOSITION: //Любое перемещение
		case SB_THUMBTRACK:
			param.ControlOption.MouseCoefficient = HIWORD(wp) / 100.0;			
			break;
		default: break;
		}		
		lpsi.nPos = (param.ControlOption.MouseCoefficient * 100);
		SetScrollInfo(hWndControlOption[COPT_MOUSE_COEFFICIENT], SB_CTL, &lpsi, TRUE);
		StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%.2f\0", param.ControlOption.MouseCoefficient);
		SetDlgItemText(hWnd, EditMouseCoefficient, szBuf);
		break;

	case WM_DESTROY:	//взывается при закрытии окна
		KillTimer(hWnd, COPT_TIMER);
		UnregisterClass(L"OptionControl", hInstOption);
		DestroyWindow(hWnd);
		return 0;
		break;

	case WM_NOTIFY:		
		break;

	case WM_TIMER:
		if (param.ControlOption.fRecv)
		{
			param.ControlOption.fRecv = FALSE;
			ReverseCalcParamFGUI();
			WriteControlOptionFGUI(hWnd);
		}
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
	TCHAR szBuf[128];

	if (param.ControlOption.fRecv)
	{
		param.ControlOption.fRecv = FALSE;
		ReverseCalcParamFGUI();
	}

	flagWriteGUI = TRUE;
	SetDlgItemInt(hWnd, EditM1MaxDegreePosition, param.ControlOption.M1.MaxDegreePosition, false);
	SetDlgItemInt(hWnd, EditM1MinDegreePosition, param.ControlOption.M1.MinDegreePosition, false);
	StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%.3f\0", param.ControlOption.M1.RotationSpeed);
	SetDlgItemText(hWnd, EditM1RotationSpeed, szBuf);
	SetDlgItemInt(hWnd, EditM1MicroStepsStepperMotor, param.ControlOption.M1.MicroStepsStepperMotor, false);	
	StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%.3f\0", param.ControlOption.M1.ReductionRatioStepperMotor);
	SetDlgItemText(hWnd, EditM1ReductionRatioStepperMotor, szBuf);

	SetDlgItemInt(hWnd, EditM2MaxDegreePosition, param.ControlOption.M2.MaxDegreePosition, false);
	SetDlgItemInt(hWnd, EditM2MinDegreePosition, param.ControlOption.M2.MinDegreePosition, false);
	StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%.3f\0", param.ControlOption.M2.RotationSpeed);
	SetDlgItemText(hWnd, EditM2RotationSpeed, szBuf);
	SetDlgItemInt(hWnd, EditM2MicroStepsStepperMotor, param.ControlOption.M2.MicroStepsStepperMotor, false);
	StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%.3f\0", param.ControlOption.M2.ReductionRatioStepperMotor);
	SetDlgItemText(hWnd, EditM2ReductionRatioStepperMotor, szBuf);

	if (param.ControlOption.FlagNoLimitStepMotor)
		SendMessage(hWndControlOption[COPT_NO_LIMIT], BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hWndControlOption[COPT_NO_LIMIT], BM_SETCHECK, BST_UNCHECKED, 0);

	StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%.2f\0", param.ControlOption.MouseCoefficient);
	SetDlgItemText(hWnd, EditMouseCoefficient, szBuf);
	//SetScrollPos(hWndControlOption[COPT_MOUSE_COEFFICIENT], SB_CTL, int(param.ControlOption.MouseCoefficient * 100), TRUE);

	WriteControlCalcParamFGUI(hWnd);

	flagWriteGUI = FALSE;
}
//------------------------------------------------------------------------------
void WriteControlCalcParamFGUI(HWND hWnd)
{
	TCHAR szBuf[128];
	float steps_360 = ((float)param.ControlOption.M1.MicroStepsStepperMotor * param.ControlOption.M1.ReductionRatioStepperMotor);
	float degree_in_step = 360.0 / steps_360;

	param.ControlOption.M1.Freq = (UINT)round(param.ControlOption.M1.RotationSpeed / degree_in_step);
	param.ControlOption.M1.Period = 1.0 / (float)param.ControlOption.M1.Freq;
	if (param.ControlOption.M1.Period < 0.001)
	{
		param.ControlOption.M1.Period = 0.001;
		param.ControlOption.M1.Freq = 1000;
	}
	param.ControlOption.M1.MaxStepsPosition = steps_360 * (float)param.ControlOption.M1.MaxDegreePosition / 360.0;
	param.ControlOption.M1.MinStepsPosition = steps_360 * (float)param.ControlOption.M1.MinDegreePosition / 360.0;

	SetDlgItemInt(hWnd, EditM1FreqSignal, param.ControlOption.M1.Freq, false);
	
	StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%.3f\0", param.ControlOption.M1.Period);	
	SetDlgItemText(hWnd, EditM1PeriodSignal, szBuf);

	steps_360 = ((float)param.ControlOption.M2.MicroStepsStepperMotor * param.ControlOption.M2.ReductionRatioStepperMotor);
	degree_in_step = 360.0 / steps_360;

	param.ControlOption.M2.Freq = (UINT)round(param.ControlOption.M2.RotationSpeed / degree_in_step);
	param.ControlOption.M2.Period = 1.0 / (float)param.ControlOption.M2.Freq;
	if (param.ControlOption.M2.Period < 0.001)
	{
		param.ControlOption.M2.Period = 0.001;
		param.ControlOption.M2.Freq = 1000;
	}
	param.ControlOption.M2.MaxStepsPosition = steps_360 * (float)param.ControlOption.M2.MaxDegreePosition / 360.0;
	param.ControlOption.M2.MinStepsPosition = steps_360 * (float)param.ControlOption.M2.MinDegreePosition / 360.0;
	
	SetDlgItemInt(hWnd, EditM2FreqSignal, param.ControlOption.M2.Freq, false);

	StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%.3f\0", param.ControlOption.M2.Period);
	SetDlgItemText(hWnd, EditM2PeriodSignal, szBuf);

	param.ControlOption.fSend = TRUE; //Посылаем параметры в турель
}
//------------------------------------------------------------------------------
void ReverseCalcParamFGUI(void)
{
	TCHAR szBuf[128];
	float fractpart;
	float steps_360 = ((float)param.ControlOption.M1.MicroStepsStepperMotor * param.ControlOption.M1.ReductionRatioStepperMotor);
	float degree_in_step = 360.0 / steps_360;

	param.ControlOption.M1.RotationSpeed = param.ControlOption.M1.Freq * degree_in_step;
	param.ControlOption.M1.Period = 1.0 / (float)param.ControlOption.M1.Freq;
	param.ControlOption.M1.MaxDegreePosition = param.ControlOption.M1.MaxStepsPosition * 360.0 / steps_360;
	param.ControlOption.M1.MinDegreePosition = param.ControlOption.M1.MinStepsPosition * 360.0 / steps_360;
	
	steps_360 = ((float)param.ControlOption.M2.MicroStepsStepperMotor * param.ControlOption.M2.ReductionRatioStepperMotor);
	degree_in_step = 360.0 / steps_360;

	param.ControlOption.M2.RotationSpeed = param.ControlOption.M2.Freq * degree_in_step;
	param.ControlOption.M2.Period = 1.0 / (float)param.ControlOption.M2.Freq;
	param.ControlOption.M2.MaxDegreePosition = param.ControlOption.M2.MaxStepsPosition * 360.0 / steps_360;
	param.ControlOption.M2.MinDegreePosition = param.ControlOption.M2.MinStepsPosition * 360.0 / steps_360;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CALLBACK EditM1RotationSpeedWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CHAR:
		if (!((wp >= '0' && wp <= '9')
			|| wp == '.'
			|| wp == ','
			|| wp == VK_RETURN
			|| wp == VK_DELETE
			|| wp == VK_BACK))
		{
			return NULL;
		}
		break;
	}

	if (wp == '.')
		wp = ',';

	return CallWindowProc(OriginalEditM1RotationSpeed, hWnd, msg, wp, lp);
}
//------------------------------------------------------------------------------
LRESULT CALLBACK EditM2RotationSpeedWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CHAR:
		if (!((wp >= '0' && wp <= '9')
			|| wp == '.'
			|| wp == ','
			|| wp == VK_RETURN
			|| wp == VK_DELETE
			|| wp == VK_BACK))
		{
			return NULL;
		}
		break;
	}

	if (wp == '.')
		wp = ',';

	return CallWindowProc(OriginalEditM2RotationSpeed, hWnd, msg, wp, lp);
}
//------------------------------------------------------------------------------
LRESULT CALLBACK EditM1ReductionRatioWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CHAR:
		if (!((wp >= '0' && wp <= '9')
			|| wp == '.'
			|| wp == ','
			|| wp == VK_RETURN
			|| wp == VK_DELETE
			|| wp == VK_BACK))
		{
			return NULL;
		}
		break;
	}

	if (wp == '.')
		wp = ',';

	return CallWindowProc(OriginalEditM1RotationSpeed, hWnd, msg, wp, lp);
}
//------------------------------------------------------------------------------
LRESULT CALLBACK EditM2ReductionRatioWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CHAR:
		if (!((wp >= '0' && wp <= '9')
			|| wp == '.'
			|| wp == ','
			|| wp == VK_RETURN
			|| wp == VK_DELETE
			|| wp == VK_BACK))
		{
			return NULL;
		}
		break;
	}

	if (wp == '.')
		wp = ',';

	return CallWindowProc(OriginalEditM1RotationSpeed, hWnd, msg, wp, lp);
}
//------------------------------------------------------------------------------
LRESULT CALLBACK MouseCoefficientWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_CHAR:
		if (!((wp >= '0' && wp <= '9')
			|| wp == '.'
			|| wp == ','
			|| wp == VK_RETURN
			|| wp == VK_DELETE
			|| wp == VK_BACK))
		{
			return NULL;
		}
		break;
	}

	if (wp == '.')
		wp = ',';

	return CallWindowProc(OriginalEditMouseCoefficient, hWnd, msg, wp, lp);
}