
#include <windows.h>

#include <emuall/support.h>
#include <GL/glew.h>
#include <assert.h>

BOOL WINAPI DLLMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

void emuallSupportInit() {
	glewExperimental = true;
	assert(glewInit() == GLEW_OK);
}