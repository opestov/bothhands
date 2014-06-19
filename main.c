/*
 * Small program that helps to use both hands for pressing
 * keyboard combinations.
 */
#include <stdio.h>
#include <time.h>

#include "resource.h"
#include "keyboard_state.h"

#define CONFIG_FILE "keys.ini"
#define ID_EXIT 2001

// registered message id
UINT WM_TRAY;

// menu to show when user clicks right mouse button on notification icon
HMENU hMenu;

// keyboard state
struct keyboard_state state;

void show_popup_menu(HWND hWnd)
{
        POINT cursor;
        GetCursorPos(&cursor);

        SetForegroundWindow(hWnd);
        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, hWnd, NULL);
        PostMessage(hWnd, WM_NULL, 0, 0);
}

// handles keyboard events
LRESULT CALLBACK kbd_proc(int nCode, WPARAM wParam, LPARAM lParam)
{
        if (nCode >= 0) {
                ks_handle_event(&state, (KBDLLHOOKSTRUCT *) lParam);
                if (ks_is_alarm_needed(&state))
                        MessageBeep(0xFFFFFFFF);
        }

        return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// handles general window events
LRESULT CALLBACK window_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
        if (uMsg == WM_TRAY) {
                if (lParam == WM_RBUTTONUP) {
                        show_popup_menu(hWnd);
                        return 0;
                }
        }
        else if (uMsg == WM_COMMAND) {
                if (LOWORD(wParam) == ID_EXIT) {
                        PostQuitMessage(0);
                        return 0;
                }
        }
        else if (uMsg == WM_DESTROY) {
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// shows cryptic message when something goes wrong
void fail(char *message)
{
        MessageBox(NULL, message, "Error",  MB_ICONEXCLAMATION | MB_OK);
        exit(EXIT_FAILURE);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
        ks_init(&state, CONFIG_FILE);

        // we are going to show tray icon only
        // main window will only handle events

        // register custom window class
        WNDCLASSEX wc = {sizeof(wc), 0, window_proc, 0, 0,
                         hInstance, NULL, NULL, NULL, NULL, "ErgoKeyClass", NULL};
        if (!RegisterClassEx(&wc))
                fail("RegisterClassEx");

        // and create "null" window
        HWND hWnd = CreateWindow(wc.lpszClassName, "title", WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
        if (hWnd == NULL)
                fail("CreateWindow");

        // register message
        WM_TRAY = RegisterWindowMessage("ErgoKeysTaskbarMessage");
        if (WM_TRAY == 0) {
                fail("RegisterWindowMessage");
        }

        // structure that specifies notification area icon and notification itself
        HICON icon = LoadIcon(hInstance, MAKEINTRESOURCE(AppIcon));
        NOTIFYICONDATA nid = {sizeof(nid), hWnd, 1, NIF_MESSAGE | NIF_ICON, WM_TRAY, icon, ""};

        // the only possible action is to close program
        hMenu = CreatePopupMenu();
        InsertMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, ID_EXIT, "Exit");

        // add hook to the chain to monitor low-level keyboard input events
        HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, kbd_proc, NULL, 0);

        // show tray icon
        Shell_NotifyIcon(NIM_ADD, &nid);

        // main cycle
        MSG message;
        while (GetMessage(&message, NULL, 0, 0)) {
                TranslateMessage(&message);
                DispatchMessage(&message);
        }

        // release resources
        Shell_NotifyIcon(NIM_DELETE, &nid);
        UnregisterClass(wc.lpszClassName, hInstance);
        UnhookWindowsHookEx(hook);
        ks_release(&state);

        return message.wParam;
}
