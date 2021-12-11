#define NOMINMAX
#include <windows.h>
#include "graphics.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constexpr uint cc0 = 0xffffffff; // цвет фона
constexpr uint cc1 = 0xff000000; // цвет линий

_bitmap paper;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void draw_the_staff()
{
	constexpr i64 delta_line = 6;

	constexpr i64 delta5 = 13 * delta_line;
	constexpr i64 staff_x_size = 125 * delta_line;
	constexpr i64 x_offset = 9 * delta_line;
	constexpr i64 staff_y_size = 28 * delta_line;

	constexpr i64 start_y = (staff_y_size - 8 * delta_line) / 4;
	i64 staff_x_count = std::max(paper.size.x / staff_x_size, 1LL);
	i64 staff_y_count = std::max(paper.size.y / staff_y_size, 1LL);
	i64 x_start = (paper.size.x - staff_x_count * staff_x_size) / 2;
	i64 y_start = (paper.size.y - staff_y_count * staff_y_size) / 2;
	paper.clear(cc0);
	for (auto p = 0; p < staff_x_count; p++)
	{
		auto x1 = x_start + p * staff_x_size + x_offset;
		auto x2 = x_start + (p + 1) * staff_x_size - x_offset;
		for (auto i = 0; i < staff_y_count; i++)
		{
			for (auto j = 0; j < 5; j++)
			{
				auto y = y_start + i * staff_y_size + start_y + j * delta_line;
				paper.line({ x1, y }, { x2, y }, cc1);
				paper.line({ x1, y + delta5 }, { x2, y + delta5 }, cc1);
			}
			auto y = y_start + i * staff_y_size + start_y;
			paper.line({ x1, y }, { x1, y + 4 * delta_line + delta5 }, cc1);
			paper.line({ x2, y }, { x2, y + 4 * delta_line + delta5 }, cc1);
			paper.text({ x1 + delta_line / 2, y - 3 * delta_line }, L"𝄞", 9 * delta_line, cc1);
			paper.text({ x1 + delta_line / 2, y - 2 * delta_line + delta5 }, L"𝄢", 7 * delta_line, cc1);
		}
	}
}

void draw(_isize r)
{
	if (!paper.resize(r)) return;
	draw_the_staff();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	paper.set_font(L"Segoe UI Symbol", false);
	static bool run_timer = true;
	static bool tracking_mouse = false;
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		HDC hdc = GetDC(hWnd);
		RECT rect;
		GetClientRect(hWnd, &rect);
		draw({ rect.right, rect.bottom });
		BitBlt(hdc, 0, 0, rect.right, rect.bottom, paper.hdc, 0, 0, SRCCOPY);
		ReleaseDC(hWnd, hdc);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	static TCHAR szWindowClass[] = L"win64app";
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101)); // 101 из resourse.h
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = 0;
	if (!RegisterClassEx(&wcex)) return 2;
	HWND hWnd = CreateWindow(szWindowClass, L"piano", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1600, 800,
		NULL, NULL, hInstance, NULL);
	if (!hWnd) return 3;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
