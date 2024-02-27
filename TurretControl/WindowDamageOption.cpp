//******************************************************************************
//include
//******************************************************************************
#include "WindowDamageOption.h"
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
static HWND hwndDamageOptionWindow;
static HWND hWndDamageOption[10];

static BOOL flagWriteGUI;
//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
LRESULT CALLBACK DamageOptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
void WriteDamageOptionFGUI(HWND hWnd);
//******************************************************************************
// Секция описания функций
//******************************************************************************
int WINAPI CreateWindow_DamageOption(HINSTANCE hInst, HWND parent)
{
	hInstOption = hInst;
	flagWriteGUI = TRUE;
	WNDCLASSEX NWC = { 0 };

	//NWC.style = CS_HREDRAW | CS_HREDRAW;
	NWC.cbSize = sizeof(WNDCLASSEX);
	NWC.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1));
	NWC.hCursor = LoadCursor(NULL, IDC_ARROW);
	NWC.hInstance = hInst;
	NWC.lpszClassName = L"OptionDamage";
	NWC.hbrBackground = (HBRUSH)COLOR_WINDOW;
	NWC.lpfnWndProc = DamageOptionWindowProcedure;
	NWC.style = CS_HREDRAW | CS_VREDRAW;

	UnregisterClass(L"OptionDamage", hInstOption);
	if (!RegisterClassEx(&NWC)) { return -1; }

	MSG SoftwareMainMessege = { 0 };

	hwndDamageOptionWindow = CreateWindow(L"OptionDamage", L"Настройки урона турели", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 300, parent, NULL, hInst, NULL);

	ShowWindow(hwndDamageOptionWindow, SW_NORMAL);
	UpdateWindow(hwndDamageOptionWindow);

	return 0;
}
//------------------------------------------------------------------------------
LRESULT CALLBACK DamageOptionWindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
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
			case CheckBoxWeapoOff:
				param.DamageOption.WeaponOff = SendMessage(hWndDamageOption[DOPT_WEAPON_OFF], BM_GETCHECK, 0, 0);
				break;
			case CheckBoxMovementOff:
				param.DamageOption.MovementOff = SendMessage(hWndDamageOption[DOPT_MOVEMENT_OFF], BM_GETCHECK, 0, 0);
				break;
			case CheckBoxVideoOff:
				param.DamageOption.VideoOff = SendMessage(hWndDamageOption[DOPT_VIDEO_OFF], BM_GETCHECK, 0, 0);
				break;
			case EditOptionHP:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditOptionHP, NULL, false);
					if (value < 1)
						param.DamageOption.HealPoint = 1;
					else if (value > 120)
						param.DamageOption.HealPoint = 120;
					else
						param.DamageOption.HealPoint = value;
					param.DamageOption.fSend = TRUE;
				}
				break;			
			case EditOptionDelayDamageMinute:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditOptionDelayDamageMinute, NULL, false);
					if (value > 60)
						param.DamageOption.DamageDelayMinute = 60;
					else
						param.DamageOption.DamageDelayMinute = value;
					param.DamageOption.fSend = TRUE;
				}
				break;
			case EditOptionDelayDamageSecunde:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditOptionDelayDamageSecunde, NULL, false);
					if (value > 59)
						param.DamageOption.DamageDelaySecond = 59;
					else
						param.DamageOption.DamageDelaySecond = value;
					param.DamageOption.fSend = TRUE;
				}
				break;
			case EditOptionDelaySensor_ms:
				if (HIWORD(wp) == EN_UPDATE)
				{
					value = GetDlgItemInt(hWnd, EditOptionDelaySensor_ms, NULL, false);
					if (value < 1)
						param.DamageOption.DelaySensor_ms = 1;
					else if (value > 10000)
						param.DamageOption.DelaySensor_ms = 10000;
					else
						param.DamageOption.DelaySensor_ms = value;
					param.DamageOption.fSend = TRUE;
				}
				break;
			}
		}
		break;
	case WM_CREATE:	//вызывается при создании окна		
		param.DamageOption.fSendReqParam = TRUE; //Запрос параметров

		CreateWindow(WC_STATIC, L"Жизни турели. 1 жизнь = 1 попадание", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_DAMAGE_OPTION * cnt, 340, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"40", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_DAMAGE_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionHP, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"point", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_DAMAGE_OPTION * cnt++, 60, 22, hWnd, NULL, hInstOption, NULL);
		cnt++;
		CreateWindow(WC_STATIC, L"При получении критического урона", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_DAMAGE_OPTION * cnt++, 340, 22, hWnd, NULL, hInstOption, NULL);
		hWndDamageOption[DOPT_WEAPON_OFF] = CreateWindow(WC_BUTTON, L"отключать стрельбу", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10 + LINE_SPACE_DAMAGE_OPTION * cnt++, 400, 22, hWnd, (HMENU)CheckBoxWeapoOff, hInstOption, NULL);
		hWndDamageOption[DOPT_MOVEMENT_OFF] = CreateWindow(WC_BUTTON, L"отключать движение", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10 + LINE_SPACE_DAMAGE_OPTION * cnt++, 400, 22, hWnd, (HMENU)CheckBoxMovementOff, hInstOption, NULL);
		hWndDamageOption[DOPT_VIDEO_OFF] = CreateWindow(WC_BUTTON, L"отключать изображение", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, 10, 10 + LINE_SPACE_DAMAGE_OPTION * cnt++, 400, 22, hWnd, (HMENU)CheckBoxVideoOff, hInstOption, NULL);
		cnt++;
		CreateWindow(WC_STATIC, L"Временя получения урона", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_DAMAGE_OPTION * cnt, 300, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"2", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 310, 10 + LINE_SPACE_DAMAGE_OPTION * cnt, 30, 22, hWnd, (HMENU)EditOptionDelayDamageMinute, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"min", WS_VISIBLE | WS_CHILD, 345, 10 + LINE_SPACE_DAMAGE_OPTION * cnt, 25, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"30", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 375, 10 + LINE_SPACE_DAMAGE_OPTION * cnt, 30, 22, hWnd, (HMENU)EditOptionDelayDamageSecunde, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"sec", WS_VISIBLE | WS_CHILD, 410, 10 + LINE_SPACE_DAMAGE_OPTION * cnt++, 60, 22, hWnd, NULL, hInstOption, NULL);
		cnt++;
		CreateWindow(WC_STATIC, L"Задержка срабатывания датчика", WS_VISIBLE | WS_CHILD, 10, 10 + LINE_SPACE_DAMAGE_OPTION * cnt, 340, 22, hWnd, NULL, hInstOption, NULL);
		CreateWindow(WC_EDIT, L"500", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 350, 10 + LINE_SPACE_DAMAGE_OPTION * cnt, 60, 22, hWnd, (HMENU)EditOptionDelaySensor_ms, hInstOption, NULL);
		CreateWindow(WC_STATIC, L"мс", WS_VISIBLE | WS_CHILD, 412, 10 + LINE_SPACE_DAMAGE_OPTION * cnt++, 60, 22, hWnd, NULL, hInstOption, NULL);
		WriteDamageOptionFGUI(hWnd);

		SetTimer(hWnd, DOPT_TIMER, 1000, NULL);

		break;
	case WM_DESTROY:	//взывается при закрытии окна		
		KillTimer(hWnd, DOPT_TIMER);
		UnregisterClass(L"OptionDamage", hInstOption);
		DestroyWindow(hWnd);
		return 0;
		break;

	case WM_NOTIFY:		
			break;

	case WM_TIMER:
		if (param.DamageOption.fRecv)
		{
			param.DamageOption.fRecv = FALSE;
			WriteDamageOptionFGUI(hWnd);
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
void WriteDamageOptionFGUI(HWND hWnd)
{
	flagWriteGUI = TRUE;

	if (param.DamageOption.WeaponOff)
		SendMessage(hWndDamageOption[DOPT_WEAPON_OFF], BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hWndDamageOption[DOPT_WEAPON_OFF], BM_SETCHECK, BST_UNCHECKED, 0);

	if (param.DamageOption.MovementOff)
		SendMessage(hWndDamageOption[DOPT_MOVEMENT_OFF], BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hWndDamageOption[DOPT_MOVEMENT_OFF], BM_SETCHECK, BST_UNCHECKED, 0);

	if (param.DamageOption.VideoOff)
		SendMessage(hWndDamageOption[DOPT_VIDEO_OFF], BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hWndDamageOption[DOPT_VIDEO_OFF], BM_SETCHECK, BST_UNCHECKED, 0);

	SetDlgItemInt(hWnd, EditOptionHP, param.DamageOption.HealPoint, false);
	SetDlgItemInt(hWnd, EditOptionDelayDamageMinute, param.DamageOption.DamageDelayMinute, false);
	SetDlgItemInt(hWnd, EditOptionDelayDamageSecunde, param.DamageOption.DamageDelaySecond, false);
	SetDlgItemInt(hWnd, EditOptionDelaySensor_ms, param.DamageOption.DelaySensor_ms, false);

	flagWriteGUI = FALSE;
}