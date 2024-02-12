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
void WriteControlCalcParamFGUI(HWND hWnd);
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
	UINT value;

	switch (msg)
	{
	case WM_COMMAND:
		if (flagWriteGUI == FALSE)
		{
			switch (LOWORD(wp))
			{
			//M1
			case EditM1RotationLimit:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM1RotationLimit, NULL, false);
					if (value >= 360)
						param.ControlOption.M1.RotationLimit = 359;
					else
						param.ControlOption.M1.RotationLimit = value;

					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM1RotationSpeedInt:
				if (HIWORD(wp) == EN_UPDATE)
				{
					param.ControlOption.M1.RotationSpeedInt = GetDlgItemInt(hWnd, EditM1RotationSpeedInt, NULL, false);
					param.ControlOption.M1.RotationSpeedFrac = GetDlgItemInt(hWnd, EditM1RotationSpeedFrac, NULL, false);
					if (param.ControlOption.M1.RotationSpeedFrac < 10) param.ControlOption.M1.RotationSpeedFrac *= 100;
					else if (param.ControlOption.M1.RotationSpeedFrac < 100) param.ControlOption.M1.RotationSpeedFrac *= 10;
					param.ControlOption.M1.RotationSpeed = (float)param.ControlOption.M1.RotationSpeedInt + (float)param.ControlOption.M1.RotationSpeedFrac / 1000.0;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM1RotationSpeedFrac:
				if (HIWORD(wp) == EN_UPDATE)
				{
					param.ControlOption.M1.RotationSpeedInt = GetDlgItemInt(hWnd, EditM1RotationSpeedInt, NULL, false);
					param.ControlOption.M1.RotationSpeedFrac = GetDlgItemInt(hWnd, EditM1RotationSpeedFrac, NULL, false);
					if (param.ControlOption.M1.RotationSpeedFrac < 10) param.ControlOption.M1.RotationSpeedFrac *= 100;
					else if (param.ControlOption.M1.RotationSpeedFrac < 100) param.ControlOption.M1.RotationSpeedFrac *= 10;
					param.ControlOption.M1.RotationSpeed = (float)param.ControlOption.M1.RotationSpeedInt + (float)param.ControlOption.M1.RotationSpeedFrac / 1000.0;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM1StepsStepperMotor:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM1StepsStepperMotor, NULL, false);
					if (value < 1)
						param.ControlOption.M1.StepsStepperMotor = 1;
					else if (value > 1000)
						param.ControlOption.M1.StepsStepperMotor = 10000;
					else
						param.ControlOption.M1.StepsStepperMotor = value;
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

			case EditM1ReductionRatioStepperMotorInt:
				if (HIWORD(wp) == EN_UPDATE)
				{
					param.ControlOption.M1.ReductionRatioStepperMotorInt = GetDlgItemInt(hWnd, EditM1ReductionRatioStepperMotorInt, NULL, false);
					param.ControlOption.M1.ReductionRatioStepperMotorFrac = GetDlgItemInt(hWnd, EditM1ReductionRatioStepperMotorFrac, NULL, false);
					if (param.ControlOption.M1.ReductionRatioStepperMotorFrac < 10) param.ControlOption.M1.ReductionRatioStepperMotorFrac *= 100;
					else if (param.ControlOption.M1.ReductionRatioStepperMotorFrac < 100) param.ControlOption.M1.ReductionRatioStepperMotorFrac *= 10;
					param.ControlOption.M1.ReductionRatioStepperMotor = (float)param.ControlOption.M1.ReductionRatioStepperMotorInt + (float)param.ControlOption.M1.ReductionRatioStepperMotorFrac / 1000.0;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM1ReductionRatioStepperMotorFrac:
				if (HIWORD(wp) == EN_UPDATE)
				{
					param.ControlOption.M1.ReductionRatioStepperMotorInt = GetDlgItemInt(hWnd, EditM1ReductionRatioStepperMotorInt, NULL, false);
					param.ControlOption.M1.ReductionRatioStepperMotorFrac = GetDlgItemInt(hWnd, EditM1ReductionRatioStepperMotorFrac, NULL, false);
					if (param.ControlOption.M1.ReductionRatioStepperMotorFrac < 10) param.ControlOption.M1.ReductionRatioStepperMotorFrac *= 100;
					else if (param.ControlOption.M1.ReductionRatioStepperMotorFrac < 100) param.ControlOption.M1.ReductionRatioStepperMotorFrac *= 10;
					param.ControlOption.M1.ReductionRatioStepperMotor = (float)param.ControlOption.M1.ReductionRatioStepperMotorInt + (float)param.ControlOption.M1.ReductionRatioStepperMotorFrac / 1000.0;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			//M2
			case EditM2RotationLimit:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM2RotationLimit, NULL, false);
					if (value >= 360)
						param.ControlOption.M2.RotationLimit = 359;
					else
						param.ControlOption.M2.RotationLimit = value;

					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM2RotationSpeedInt:
				if (HIWORD(wp) == EN_UPDATE)
				{
					param.ControlOption.M2.RotationSpeedInt = GetDlgItemInt(hWnd, EditM2RotationSpeedInt, NULL, false);
					param.ControlOption.M2.RotationSpeedFrac = GetDlgItemInt(hWnd, EditM2RotationSpeedFrac, NULL, false);
					if (param.ControlOption.M2.RotationSpeedFrac < 10) param.ControlOption.M2.RotationSpeedFrac *= 100;
					else if (param.ControlOption.M2.RotationSpeedFrac < 100) param.ControlOption.M2.RotationSpeedFrac *= 10;
					param.ControlOption.M2.RotationSpeed = (float)param.ControlOption.M2.RotationSpeedInt + (float)param.ControlOption.M2.RotationSpeedFrac / 1000.0;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM2RotationSpeedFrac:
				if (HIWORD(wp) == EN_UPDATE)
				{
					param.ControlOption.M2.RotationSpeedInt = GetDlgItemInt(hWnd, EditM2RotationSpeedInt, NULL, false);
					param.ControlOption.M2.RotationSpeedFrac = GetDlgItemInt(hWnd, EditM2RotationSpeedFrac, NULL, false);
					if (param.ControlOption.M2.RotationSpeedFrac < 10) param.ControlOption.M2.RotationSpeedFrac *= 100;
					else if (param.ControlOption.M2.RotationSpeedFrac < 100) param.ControlOption.M2.RotationSpeedFrac *= 10;
					param.ControlOption.M2.RotationSpeed = (float)param.ControlOption.M2.RotationSpeedInt + (float)param.ControlOption.M2.RotationSpeedFrac / 1000.0;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM2StepsStepperMotor:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditM2StepsStepperMotor, NULL, false);
					if (value < 1)
						param.ControlOption.M2.StepsStepperMotor = 1;
					else if (value > 1000)
						param.ControlOption.M2.StepsStepperMotor = 10000;
					else
						param.ControlOption.M2.StepsStepperMotor = value;
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

			case EditM2ReductionRatioStepperMotorInt:
				if (HIWORD(wp) == EN_UPDATE)
				{
					param.ControlOption.M2.ReductionRatioStepperMotorInt = GetDlgItemInt(hWnd, EditM2ReductionRatioStepperMotorInt, NULL, false);
					param.ControlOption.M2.ReductionRatioStepperMotorFrac = GetDlgItemInt(hWnd, EditM2ReductionRatioStepperMotorFrac, NULL, false);
					if (param.ControlOption.M2.ReductionRatioStepperMotorFrac < 10) param.ControlOption.M2.ReductionRatioStepperMotorFrac *= 100;
					else if (param.ControlOption.M2.ReductionRatioStepperMotorFrac < 100) param.ControlOption.M2.ReductionRatioStepperMotorFrac *= 10;
					param.ControlOption.M2.ReductionRatioStepperMotor = (float)param.ControlOption.M2.ReductionRatioStepperMotorInt + (float)param.ControlOption.M2.ReductionRatioStepperMotorFrac / 1000.0;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			case EditM2ReductionRatioStepperMotorFrac:
				if (HIWORD(wp) == EN_UPDATE)
				{
					param.ControlOption.M2.ReductionRatioStepperMotorInt = GetDlgItemInt(hWnd, EditM2ReductionRatioStepperMotorInt, NULL, false);
					param.ControlOption.M2.ReductionRatioStepperMotorFrac = GetDlgItemInt(hWnd, EditM2ReductionRatioStepperMotorFrac, NULL, false);
					if (param.ControlOption.M2.ReductionRatioStepperMotorFrac < 10) param.ControlOption.M2.ReductionRatioStepperMotorFrac *= 100;
					else if (param.ControlOption.M2.ReductionRatioStepperMotorFrac < 100) param.ControlOption.M2.ReductionRatioStepperMotorFrac *= 10;
					param.ControlOption.M2.ReductionRatioStepperMotor = (float)param.ControlOption.M2.ReductionRatioStepperMotorInt + (float)param.ControlOption.M2.ReductionRatioStepperMotorFrac / 1000.0;
					WriteControlCalcParamFGUI(hWnd);
				}
				break;
			}
		}
		break;
	case WM_CREATE:	//вызывается при создании окна	
		CreateWindow(WC_STATIC, L"1 двигатель", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 150, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Предел вращения", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"180", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1RotationLimit, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"град", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Скорость вращения", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 200, 25, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"1", WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 30, 25, hWnd, (HMENU)EditM1RotationSpeedInt, hInstOption, NULL);
		CreateWindow(WC_STATIC, L".", WS_VISIBLE | WS_CHILD, 380, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 5, 25, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_LEFT | ES_NUMBER, 385, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 30, 25, hWnd, (HMENU)EditM1RotationSpeedFrac, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"град/сек", WS_VISIBLE | WS_CHILD, 416, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Количество шагов двигателя на оборот", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"20", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1StepsStepperMotor, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"шаг.", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Микрошагов на один шаг двигателя", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 150, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"64", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1MicroStepsStepperMotor, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"мик.шаг.", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Коэффициент редукции редуктора", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"2", WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 30, 25, hWnd, (HMENU)EditM1ReductionRatioStepperMotorInt, hInstOption, NULL);
		CreateWindow(WC_STATIC, L".", WS_VISIBLE | WS_CHILD, 380, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 5, 25, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"5", WS_VISIBLE | WS_CHILD | ES_LEFT | ES_NUMBER, 385, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 30, 25, hWnd, (HMENU)EditM1ReductionRatioStepperMotorFrac, hInstOption, NULL);
		CreateWindow(WC_STATIC, L" ", WS_VISIBLE | WS_CHILD, 416, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Частота сигнала", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1FreqSignal, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"Гц", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Период сигнала", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM1PeriodSignal, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"с", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"2 двигатель", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 150, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Предел вращения", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 300, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"180", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2RotationLimit, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"град", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Скорость вращения", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 200, 25, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"1", WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 30, 25, hWnd, (HMENU)EditM2RotationSpeedInt, hInstOption, NULL);
		CreateWindow(WC_STATIC, L".", WS_VISIBLE | WS_CHILD, 380, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 5, 25, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_LEFT | ES_NUMBER, 385, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 30, 25, hWnd, (HMENU)EditM2RotationSpeedFrac, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"град/сек", WS_VISIBLE | WS_CHILD, 416, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Количество шагов двигателя на оборот", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"20", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2StepsStepperMotor, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"шаг.", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Микрошагов на один шаг двигателя", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 150, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"64", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2MicroStepsStepperMotor, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"мик.шаг.", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Коэффициент редукции редуктора", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"2", WS_VISIBLE | WS_CHILD | ES_RIGHT | ES_NUMBER, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 30, 25, hWnd, (HMENU)EditM2ReductionRatioStepperMotorInt, hInstOption, NULL);
		CreateWindow(WC_STATIC, L".", WS_VISIBLE | WS_CHILD, 380, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 5, 25, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"5", WS_VISIBLE | WS_CHILD | ES_LEFT | ES_NUMBER, 385, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 30, 25, hWnd, (HMENU)EditM2ReductionRatioStepperMotorFrac, hInstOption, NULL);
		CreateWindow(WC_STATIC, L" ", WS_VISIBLE | WS_CHILD, 416, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Частота сигнала", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2FreqSignal, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"Гц", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

		CreateWindow(WC_STATIC, L"Период сигнала", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 250, 40, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"0", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_READONLY, 350, 10 + LINE_SPACE_CONTROL_OPTION * cnt, 60, 25, hWnd, (HMENU)EditM2PeriodSignal, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"с", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_CONTROL_OPTION * cnt++, 60, 40, hWnd, NULL, hInstOption, NULL);

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
	SetDlgItemInt(hWnd, EditM1RotationLimit, param.ControlOption.M1.RotationLimit, false);
	SetDlgItemInt(hWnd, EditM1RotationSpeedInt, param.ControlOption.M1.RotationSpeedInt, false);
	SetDlgItemInt(hWnd, EditM1RotationSpeedFrac, param.ControlOption.M1.RotationSpeedFrac, false);
	SetDlgItemInt(hWnd, EditM1StepsStepperMotor, param.ControlOption.M1.StepsStepperMotor, false);
	SetDlgItemInt(hWnd, EditM1MicroStepsStepperMotor, param.ControlOption.M1.MicroStepsStepperMotor, false);
	SetDlgItemInt(hWnd, EditM1ReductionRatioStepperMotorInt, param.ControlOption.M1.ReductionRatioStepperMotorInt, false);
	SetDlgItemInt(hWnd, EditM1ReductionRatioStepperMotorFrac, param.ControlOption.M1.ReductionRatioStepperMotorFrac, false);

	SetDlgItemInt(hWnd, EditM2RotationLimit, param.ControlOption.M2.RotationLimit, false);
	SetDlgItemInt(hWnd, EditM2RotationSpeedInt, param.ControlOption.M2.RotationSpeedInt, false);
	SetDlgItemInt(hWnd, EditM2RotationSpeedFrac, param.ControlOption.M2.RotationSpeedFrac, false);
	SetDlgItemInt(hWnd, EditM2StepsStepperMotor, param.ControlOption.M2.StepsStepperMotor, false);
	SetDlgItemInt(hWnd, EditM2MicroStepsStepperMotor, param.ControlOption.M2.MicroStepsStepperMotor, false);
	SetDlgItemInt(hWnd, EditM2ReductionRatioStepperMotorInt, param.ControlOption.M2.ReductionRatioStepperMotorInt, false);
	SetDlgItemInt(hWnd, EditM2ReductionRatioStepperMotorFrac, param.ControlOption.M2.ReductionRatioStepperMotorFrac, false);

	WriteControlCalcParamFGUI(hWnd);

	flagWriteGUI = FALSE;
}
//------------------------------------------------------------------------------
void WriteControlCalcParamFGUI(HWND hWnd)
{
	TCHAR szBuf[128];
	float degree_in_step = 360.0 / ((float)param.ControlOption.M1.StepsStepperMotor * (float)param.ControlOption.M1.MicroStepsStepperMotor * param.ControlOption.M1.ReductionRatioStepperMotor);

	param.ControlOption.M1.Freq = (UINT)round(param.ControlOption.M1.RotationSpeed / degree_in_step);
	param.ControlOption.M1.Period = 1.0 / (float)param.ControlOption.M1.Freq;
	if (param.ControlOption.M1.Period < 0.001)
	{
		param.ControlOption.M1.Period = 0.001;
		param.ControlOption.M1.Freq = 1000;
	}
	StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%.3f\0", param.ControlOption.M1.Period);

	SetDlgItemInt(hWnd, EditM1FreqSignal, param.ControlOption.M1.Freq, false);
	SetDlgItemText(hWnd, EditM1PeriodSignal, szBuf);

	degree_in_step = 360.0 / ((float)param.ControlOption.M2.StepsStepperMotor * (float)param.ControlOption.M2.MicroStepsStepperMotor * param.ControlOption.M2.ReductionRatioStepperMotor);

	param.ControlOption.M2.Freq = (UINT)round(param.ControlOption.M2.RotationSpeed / degree_in_step);
	param.ControlOption.M2.Period = 1.0 / (float)param.ControlOption.M2.Freq;
	if (param.ControlOption.M2.Period < 0.001)
	{
		param.ControlOption.M2.Period = 0.001;
		param.ControlOption.M2.Freq = 1000;
	}
	StringCchPrintf(szBuf, sizeof(szBuf) / sizeof(szBuf[0]), L"%.3f\0", param.ControlOption.M2.Period);

	SetDlgItemInt(hWnd, EditM2FreqSignal, param.ControlOption.M2.Freq, false);
	SetDlgItemText(hWnd, EditM2PeriodSignal, szBuf);
}