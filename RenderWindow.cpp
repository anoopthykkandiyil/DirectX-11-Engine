#include "WindowContainer.h"



bool RenderWindow::Initialize(
	WindowContainer* pWindowContainer,
	HINSTANCE hInstance,
	std::string window_title,
	std::string window_class,
	int width,
	int height)	
{
	this->hInstance = hInstance;
	this->width = width;
	this->height = height;
	this->window_title = window_title;
	this->window_title_wide = StringConverter::StringToWide(this->window_title);
	this->window_class = window_class;
	this->window_class_wide = StringConverter::StringToWide(this->window_class);

	this->RegisterWindowClass();

	this->handle = CreateWindowEx(0,
		this->window_class_wide.c_str(),
		this->window_title_wide.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		0, // Window X origin
		0, // Window Y origin
		this->width, // window width
		this->height, // window height
		NULL, // Handle to the parent of this window. Since this is the first window it has no parent
		NULL, // Handle to menu or child window identifier
		this->hInstance, // Handle to instance of module to be used with this window
		pWindowContainer); // Parameter to create window

	if (this->handle == NULL) {
		ErrorLogger::Log(GetLastError(), "Create WindowEX Failed for window: " + this->window_title);
		return false;
	}

	// Bring the window up to the screen and set is as main focus
	ShowWindow(this->handle, SW_SHOW);
	SetForegroundWindow(this->handle);
	SetFocus(this->handle);

	return true;
}

RenderWindow::~RenderWindow() {
	if (this->handle != NULL) {
		UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
		DestroyWindow(handle);
	}
}

LRESULT CALLBACK HandleMessageRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;


	default:
	{
		WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	}
}

LRESULT CALLBACK HandleMessageSetUp(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
	case WM_NCCREATE:
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
		if (pWindow == nullptr) {
			ErrorLogger::Log("Critical Error: Pointer to window container is null during WM_NCCREATE.\n");
			exit(-1);
		}
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMessageRedirect));
		return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

void RenderWindow::RegisterWindowClass() {
	WNDCLASSEX wc; // Our window class this has to be filled before a window can be created
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // Flags [Redraw on width/height change from resize/movement]
	wc.lpfnWndProc = HandleMessageSetUp; //	Pointer to Window Procedure function for handling function from this window
	wc.cbClsExtra = 0;  // number of extra bytes to be allotted after the window-class structure
	wc.cbWndExtra = 0;  // number of extra bytes to be allocated after the window instance
	wc.hInstance = this->hInstance;
	wc.hIcon = NULL; // Handle to the class Icon. Must be handle to an icon resource.
	wc.hIconSm = NULL; // Handle to the small Icon. We are not currently assigned an icon.
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //Default cursor
	wc.hbrBackground = NULL; // Handle to the class background brush for the window's background color
	wc.lpszMenuName = NULL; // Pointer to a null terminated character string for the menu
	wc.lpszClassName = this->window_class_wide.c_str(); // Pointer to a null terminated string of our class name
	wc.cbSize = sizeof(WNDCLASSEX); // Need to fill in the size of our struct for cbSize
	RegisterClassEx(&wc); // Register the class so that it is usable.
}

bool RenderWindow::ProcessMessages() {
	// Handle the window messages
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG)); // Initialize the message structure
	while (PeekMessage(&msg, // where to source message
		this->handle, // Handle to window we are checking messages for
		0,  // Minimum Filter message value
		0,  // Maximum filter message value
		PM_REMOVE)) { // remove messages after capturing it via peek message
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Check if the window is closed
	if (msg.message == WM_NULL) {
		if (!IsWindow(this->handle)) {
			this->handle = NULL; // Message processing loop takes care of destroying this window
			UnregisterClass(this->window_class_wide.c_str(), this->hInstance);
			return false;
		}
	}

	return true;
}

HWND RenderWindow::GetHWND() const
{
	return this->handle;
}
