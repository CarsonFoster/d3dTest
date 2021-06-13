#include "App.h"
#include "CwfException.h"
#include "Window.h"
#include <Windows.h>

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd
) {
	/* 
	* If the Window creation fails, there is no Window to own the message box, so the exceptions
	* go to Window::createExceptionMessageBoxStatic. The App class handles its exceptions in run().
	*/

	try {
		App d3dTest{ hInstance };
		d3dTest.run();
	} catch (const CwfException& e) {
		Window::createExceptionMessageBoxStatic(e);
	} catch (const std::exception& e) {
		Window::createExceptionMessageBoxStatic(e);
	} catch (...) {
		Window::createExceptionMessageBoxStatic(CWF_EXCEPTION(CwfException::Type::OTHER, L"Unknown exception occurred."));
	}

	return 0;
}