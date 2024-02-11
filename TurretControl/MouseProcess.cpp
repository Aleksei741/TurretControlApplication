//******************************************************************************
// Include
//******************************************************************************
#include "MouseProcess.h"
#include <stdlib.h>
//******************************************************************************
// Section for determining the variables used in the module
//******************************************************************************
//------------------------------------------------------------------------------
// Global
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Local
//------------------------------------------------------------------------------
static BOOL flagGetModuleHandle = FALSE;
static CHAR flagHookActive = FALSE;
static MYHOOKDATA myhookdata;
static struct MOUSE_STATUStypedef MouseStatus_fromDrv;
static BOOL FlagMouseProcActive = FALSE;
static POINT CursorZeroPosition;

static HANDLE hMutexMouseProc;
static HANDLE hSemaphoreMouseProc;
static HANDLE hTreadMouseProc;

static INT IncrementedCursor_x;
static INT IncrementedCursor_y;
//******************************************************************************
// Section of prototypes of local functions
//******************************************************************************
DWORD WINAPI MouseProcedure(CONST LPVOID lpParam);
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI SetMouseHook(HWND hwndMain, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI DelMouseHook(HWND hwndMain, UINT uMsg, WPARAM wParam, LPARAM lParam);

void (*MouseHookInterruptProcessing)(INT lr, INT ud, BOOL fAttack) = NULL;
//******************************************************************************
// Section of the description of functions
//******************************************************************************
UCHAR MouseProcess_init(void)
{
    
    FlagMouseProcActive = FALSE;

    hSemaphoreMouseProc = CreateSemaphore(
        NULL,           // default security attributes
        0,				// initial count
        1,				// maximum count
        NULL);          // unnamed semaphore

    hMutexMouseProc = CreateMutex(
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex

    hTreadMouseProc = CreateThread(NULL, NULL, MouseProcedure, NULL, NULL, NULL);

    return 0;
}
//------------------------------------------------------------------------------
UCHAR MouseProcess_deinit(void)
{
    if (hSemaphoreMouseProc) CloseHandle(hSemaphoreMouseProc);
    if (hMutexMouseProc) CloseHandle(hMutexMouseProc);
    if (hTreadMouseProc) CloseHandle(hTreadMouseProc);

    DelMouseHook(NULL, NULL, NULL, NULL);
    FlagMouseProcActive = FALSE;

    return 0;
}
//------------------------------------------------------------------------------
UCHAR MouseProcess_start(HWND hwndMain)
{   
    RECT rect;
    POINT ptClientUL;              // client upper left corner 
    POINT ptClientLR;              // client lower right corner 

    GetClientRect(hwndMain, &rect);

    ptClientUL.x = rect.left;
    ptClientUL.y = rect.top;
    ptClientLR.x = rect.right + 1;
    ptClientLR.y = rect.bottom + 1;

    ClientToScreen(hwndMain, &ptClientUL);
    ClientToScreen(hwndMain, &ptClientLR);

    SetRect(&rect, ptClientUL.x, ptClientUL.y,
        ptClientLR.x, ptClientLR.y);
    ClipCursor(&rect);

    CursorZeroPosition.x = (ptClientLR.x + ptClientUL.x) / 2;
    CursorZeroPosition.y = (ptClientLR.y + ptClientUL.y) / 2;

    SetCursorPos(CursorZeroPosition.x, CursorZeroPosition.y);

    ShowCursor(NULL);

    SetMouseHook(NULL, NULL, NULL, NULL);
    FlagMouseProcActive = TRUE;
    ResumeThread(hTreadMouseProc);

    return 0;
}
//------------------------------------------------------------------------------
UCHAR MouseProcess_stop(void)
{
    DelMouseHook(NULL, NULL, NULL, NULL);
    FlagMouseProcActive = FALSE;

    ClipCursor(NULL);
    //SetCursor(LoadCursor(NULL, IDC_ARROW));
    ShowCursor(TRUE);
    ReleaseCapture();

    return 0;
}
//------------------------------------------------------------------------------
DWORD WINAPI MouseProcedure(CONST LPVOID lpParam)
{
    int cnt = 0;
    while (1)
    {
        if(!FlagMouseProcActive)
            SuspendThread(hTreadMouseProc);

        IncrementedCursor_x = IncrementedCursor_x + (MouseStatus_fromDrv.xPosition - CursorZeroPosition.x);
        IncrementedCursor_y = IncrementedCursor_y + (MouseStatus_fromDrv.yPosition - CursorZeroPosition.y);

        SetCursorPos(CursorZeroPosition.x, CursorZeroPosition.y);
        MouseStatus_fromDrv.xPosition = CursorZeroPosition.x;
        MouseStatus_fromDrv.yPosition = CursorZeroPosition.y;

        cnt++;

        if (cnt >= 4)
        {
            if (MouseHookInterruptProcessing) MouseHookInterruptProcessing(IncrementedCursor_x, IncrementedCursor_y, MouseStatus_fromDrv.LeftButton);
            IncrementedCursor_x = 0;
            IncrementedCursor_y = 0;
            cnt = 0;
        }

        Sleep(25);        
    }

    return 0;
}
//------------------------------------------------------------------------------
LRESULT WINAPI SetMouseHook(HWND hwndMain, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int index;
    static HINSTANCE hInstance;

    if (flagGetModuleHandle == FALSE)
    {
        hInstance = GetModuleHandle(NULL);
        flagGetModuleHandle = TRUE;
    }

    myhookdata.nType = WH_MOUSE_LL;
    myhookdata.hkprc = MouseProc;

    if (flagHookActive == FALSE)
    {
        myhookdata.hhook = SetWindowsHookEx(myhookdata.nType, myhookdata.hkprc, (HINSTANCE)hInstance, NULL/*GetCurrentThreadId()*/);
        flagHookActive = TRUE;
    }
    
    return NULL;
}
//------------------------------------------------------------------------------
LRESULT WINAPI DelMouseHook(HWND hwndMain, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (flagHookActive == TRUE)
    {
        UnhookWindowsHookEx(myhookdata.hhook);
        flagHookActive = FALSE;
    }
    return NULL;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/****************************************************************
  WH_MOUSE hook procedure
 ****************************************************************/
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam; // WH_MOUSE_LL struct

    if (nCode < 0)  // do not process the message 
        return CallNextHookEx(myhookdata.hhook, nCode, wParam, lParam);


    if (nCode == 0) // we have information in wParam/lParam ? If yes, let's check it:
    {
        MouseStatus_fromDrv.xPosition = pMouseStruct->pt.x;
        MouseStatus_fromDrv.yPosition = pMouseStruct->pt.y;

        switch (wParam)
        {
        case WM_LBUTTONUP:
            MouseStatus_fromDrv.LeftButton = 0;
            if (MouseHookInterruptProcessing) MouseHookInterruptProcessing(IncrementedCursor_x, IncrementedCursor_y, MouseStatus_fromDrv.LeftButton);
            IncrementedCursor_x = 0;
            IncrementedCursor_y = 0;
            break;
        case WM_LBUTTONDOWN:
            MouseStatus_fromDrv.LeftButton = 1;
            if (MouseHookInterruptProcessing) MouseHookInterruptProcessing(IncrementedCursor_x, IncrementedCursor_y, MouseStatus_fromDrv.LeftButton);
            IncrementedCursor_x = 0;
            IncrementedCursor_y = 0;
            break;
        case WM_RBUTTONUP:
            MouseStatus_fromDrv.RightButoon = 0;
            break;
        case WM_RBUTTONDOWN:
            MouseStatus_fromDrv.RightButoon = 1;
            break;
        }
    }
    return CallNextHookEx(myhookdata.hhook, nCode, wParam, lParam);
}
//------------------------------------------------------------------------------
