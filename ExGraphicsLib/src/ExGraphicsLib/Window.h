#pragma once

#include <eglpch.h>

namespace ExGraphicsLib {

	class WindowRenderer {
	private:
		struct PaintTools {
			HDC hdc;
			HDC bitmapDc;
			HBITMAP bitmap;

			PaintTools() = default;
		};

		HWND hwnd;
		POINT size;
		RECT clientRect;
		std::unique_ptr<PaintTools> paint;

	public:
		WindowRenderer(const HWND& hwnd);

		void beginPaint();
		void renderPixels(const std::pair<COLORREF, POINT>* const pixels, const size_t& len);
		void endPaint();

		void resize(const POINT& size);
		const POINT& getSize() const;
	};

	class Window {
	private:
		using TickFunc = void(*)(const Window&);

		bool nameSet;
		bool created;
		WNDPROC func;
		TickFunc tickFunc;
		WNDCLASSA WndClass;
		HWND hwnd;
		std::shared_ptr<WindowRenderer> renderer;

	public:
		Window();

		void setClassName(const std::string& name);
		void setProcFunc(const WNDPROC func);
		void setTickFunc(const TickFunc func);

		void create(const std::string& windowName = "Window", const int& width = 600, const int& height = 400, const HWND* parent = nullptr);
		void loop();

		std::weak_ptr<WindowRenderer> getRenderer() const;

		LRESULT standardProcFunc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};

}