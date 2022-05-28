#include <eglpch.h>

#include "Window.h"

ExGraphicsLib::Window::Window() :
	WndClass(), nameSet(false), created(false), hwnd(NULL)
{}

void ExGraphicsLib::Window::setClassName(const std::string& name) {
	nameSet = true;
	WndClass.lpszClassName = name.c_str();
}

void ExGraphicsLib::Window::setProcFunc(const WNDPROC func) {
	WndClass.lpfnWndProc = func;
}

void ExGraphicsLib::Window::setTickFunc(const TickFunc func) {
	tickFunc = func;
}

void ExGraphicsLib::Window::create(const std::string& windowName, const int& width, const int& height, const HWND* parent) {
	WndClass.lpszMenuName = NULL;
	WndClass.hInstance = NULL;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hbrBackground = (HBRUSH) COLOR_WINDOW;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpszClassName = (nameSet) ? WndClass.lpszClassName : "RenderWindowClass";

	if (!RegisterClassA(&WndClass)) {
		std::cout << "Error: Window class not registered" << std::endl;
		return;
	}

	hwnd = CreateWindowA(
		WndClass.lpszClassName,
		windowName.c_str(),
		WS_OVERLAPPEDWINDOW,
		200,
		200,
		width,
		height,
		(parent) ? *parent : NULL,
		NULL,
		NULL,
		NULL
	);

	ShowWindow(hwnd, SW_SHOWNORMAL);
	renderer = std::make_shared<WindowRenderer>(hwnd);
	created = true;

	PostMessage(hwnd, WM_SIZE, NULL, MAKELPARAM(width, height));
}

void ExGraphicsLib::Window::loop() {

	if (created) {
		MSG msg;
		while (true) {
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				if (msg.message == WM_QUIT) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			} else if (tickFunc != nullptr && renderer.get() != nullptr) {
				tickFunc(*this);
			}
		}
	}

}

std::weak_ptr<ExGraphicsLib::WindowRenderer> ExGraphicsLib::Window::getRenderer() const {
	return renderer;
}

LRESULT ExGraphicsLib::Window::standardProcFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		case WM_SIZE:
			if (renderer) renderer->resize({ LOWORD(lParam), HIWORD(lParam) });
			break;

		default:
			return DefWindowProcA(hwnd, msg, wParam, lParam);
	}
}

ExGraphicsLib::WindowRenderer::WindowRenderer(const HWND& hwnd) :
	hwnd(hwnd), size({ 0, 0 })
{}

void ExGraphicsLib::WindowRenderer::beginPaint() {
	paint = std::make_unique<PaintTools>();
	paint->hdc = GetDC(hwnd);
	paint->bitmapDc = CreateCompatibleDC(paint->hdc);
	paint->bitmap = CreateCompatibleBitmap(paint->hdc, clientRect.right - clientRect.left,clientRect.bottom - clientRect.top);

	SelectObject(paint->bitmapDc, paint->bitmap);
}

void ExGraphicsLib::WindowRenderer::renderPixels(const std::pair<COLORREF, POINT>* const pixels, const size_t & len) {

	for (size_t i = 0; i < len; i++) {
		const std::pair<COLORREF, POINT>& data = pixels[i];
		SetPixel(paint->bitmapDc, data.second.x, data.second.y, data.first);
	}

	BitBlt(paint->hdc, 0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, paint->bitmapDc, 0, 0, SRCCOPY);
}

void ExGraphicsLib::WindowRenderer::endPaint() {
	DeleteDC(paint->bitmapDc);
	DeleteObject(paint->bitmap);
	ReleaseDC(hwnd, paint->hdc);
	paint = nullptr;
}

void ExGraphicsLib::WindowRenderer::resize(const POINT& size) {
	this->size = size;
	GetClientRect(hwnd, &clientRect);
}

const POINT& ExGraphicsLib::WindowRenderer::getSize() const {
	return size;
}
