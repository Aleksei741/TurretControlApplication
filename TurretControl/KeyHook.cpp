//******************************************************************************
//include
//******************************************************************************
#include "main.h"
#include "KeyHook.h"
#include <Windows.h>
#include <chrono>
//******************************************************************************
// Секция определения переменных, используемых в модуле
//******************************************************************************
//------------------------------------------------------------------------------
// Глобальные
//------------------------------------------------------------------------------
void (*CallbackKeyHook)(BOOL fLeft, BOOL fRight, BOOL fUp, BOOL fDown, BOOL fAttack);
//------------------------------------------------------------------------------
// Локальные
//------------------------------------------------------------------------------
static HHOOK hhook;

//HANDLE hTreadKeyHook;

static UCHAR KeyDown;
static UCHAR KeyUp;
//******************************************************************************
// Секция прототипов локальных функций
//******************************************************************************
LRESULT WINAPI KeyboardCallback(int, WPARAM, LPARAM);
//******************************************************************************
// Секция описания функций
//******************************************************************************
LRESULT WINAPI SetKeyboardHook(HWND hwndMain, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int index;
    HINSTANCE hInstance = GetModuleHandle(NULL);        
    hhook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardCallback, (HINSTANCE)hInstance, NULL/*GetCurrentThreadId()*/);
    //hTreadKeyHook = CreateThread(NULL, NULL, SendCommandProcedure, NULL, NULL, NULL);

    return NULL;
}
//------------------------------------------------------------------------------
LRESULT WINAPI DelKeyboardHook(HWND hwndMain, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int index;

    UnhookWindowsHookEx(hhook);

    return NULL;
}
//------------------------------------------------------------------------------

/****************************************************************
  WH_KEYBOARD hook procedure
 ****************************************************************/
LRESULT CALLBACK KeyboardCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
    static BOOL fLeft;
    static BOOL fRight;
    static BOOL fUp;
    static BOOL fDown;
    static BOOL fAttack;

    if (nCode < 0)  // do not process message 
        return CallNextHookEx(hhook, nCode, wParam, lParam);

    if (HC_ACTION == nCode)
    {
        switch (wParam)
        {
        case WM_KEYDOWN:
            switch (p->vkCode)
            {
            case VK_UP:
                fUp = TRUE;
                break;
            case VK_DOWN:
                fDown = TRUE;
                break;
            case VK_LEFT:
                fLeft = TRUE;
                break;
            case VK_RIGHT:
                fRight = TRUE;
                break;
            case VK_SPACE:
                 fAttack = TRUE;
                break;
            }
            break;

        case WM_KEYUP:
            switch (p->vkCode)
            {
            case VK_UP:
                fUp = FALSE;
                break;
            case VK_DOWN:
                fDown = FALSE;
                break;
            case VK_LEFT:
                fLeft = FALSE;
                break;
            case VK_RIGHT:
                fRight = FALSE;
                break;
            case VK_SPACE:
                fAttack = FALSE;
                break;
            }
            break;
        }
    }

    if(CallbackKeyHook) CallbackKeyHook(fLeft, fRight, fUp, fDown, fAttack);
    
    return CallNextHookEx(hhook, nCode, wParam, lParam);
}
