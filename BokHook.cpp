#include "stdafx.h"

LRESULT _stdcall BokDllHookLL(int code, WPARAM wParam, LPARAM lParam);
BOOL lock = TRUE;
BOOL lock_enter = TRUE;
BOOL lock_neg = TRUE;

LRESULT _stdcall BokDllHookLL(int code, WPARAM wParam, LPARAM lParam) {
	if(!lock) return CallNextHookEx(NULL,code,wParam,lParam);
	if(code < 0) return CallNextHookEx(NULL,code,wParam,lParam);
	if(code != HC_ACTION) return CallNextHookEx(NULL,code,wParam,lParam);
	LPKBDLLHOOKSTRUCT st = (LPKBDLLHOOKSTRUCT)lParam;
	DWORD vkCode = st->vkCode;
#if 0
	if(wParam==WM_SYSKEYUP||wParam==WM_SYSKEYDOWN) {
		if(vkCode!=VK_LWIN && vkCode!=VK_RWIN) return CallNextHookEx(NULL,code,wParam,lParam);
		return 1;
	}
#endif
	if(!lock_neg) {
		DWORD ig[] = {VK_PRIOR,VK_NEXT,VK_END,VK_HOME,
			VK_LEFT,VK_UP,VK_RIGHT,VK_DOWN};
		for(int i = 0; i < sizeof(ig)/sizeof(int); ++i) {
			DWORD k = ig[i];
			if(k == vkCode) return CallNextHookEx(NULL,code,wParam,lParam);
		}
	}
	if(!lock_enter && vkCode == VK_RETURN) return CallNextHookEx(NULL,code,wParam,lParam);
	
	return 1;
}


