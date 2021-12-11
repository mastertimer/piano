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
	constexpr i64 x_offset = 10;
	constexpr i64 delta_line = 3;
	constexpr i64 start_y = delta_line * 3;
	constexpr i64 staff_size = delta_line * 10;
	const i64 staff_count = paper.size.y / staff_size;
	paper.clear(cc0);
	for (auto i = 0; i < staff_count; i++)
	{
		for (auto j = 0; j < 5; j++)
		{
			auto y = i * staff_size + start_y + j * delta_line;
			paper.line({ x_offset, y }, { paper.size.x - x_offset, y }, cc1);
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
	wchar_t buffer[MAX_PATH];
	GetModuleFileName(hInstance, buffer, MAX_PATH);

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
	HWND hWnd = CreateWindow(szWindowClass, L"mutator", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1600, 800,
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
