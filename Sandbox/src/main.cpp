#include <iostream>
#include <ExGraphicsLib/ExGraphicsLib.h>

LRESULT WINAPI Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void tick(const ExGraphicsLib::Window& wind);

std::unique_ptr<ExGraphicsLib::Window> window;

int main() {
	window = std::make_unique<ExGraphicsLib::Window>();
	window->setProcFunc(Proc);
	window->setTickFunc(tick);
	window->create();
	window->loop();

	std::cin.get();

	return 0;
}

LRESULT WINAPI Proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	return window->standardProcFunc(hwnd, msg, wParam, lParam);
}

void tick(const ExGraphicsLib::Window& wind) {
	using namespace ExGraphicsLib;

	std::shared_ptr<WindowRenderer> renderer = wind.getRenderer().lock();

	renderer->beginPaint();

	std::pair<COLORREF, POINT> pix = std::make_pair(RGB(255, 255, 255), POINT({ 10, 10 }));
	renderer->renderPixels(&pix, 1);

	renderer->endPaint();

}