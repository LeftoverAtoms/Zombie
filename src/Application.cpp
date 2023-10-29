// TODO: Abstract uncomprehensible win32 bullshittery.

#ifndef UNICODE
#define UNICODE
#endif 

#include "Game.h"
#include <stdint.h>
#include <windows.h>

const wchar_t WindowTitle[] = L"Zombie Game";

static bool quit = false;

struct
{
	int Width;
	int Height;
	uint32_t* Pixels;
} Frame;

{
	// Convert coordinate space to pixel space.

	{
		{
		}
	}
}

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

static BITMAPINFO BitmapInfo;
static HBITMAP Bitmap;
static HDC DeviceContextHandle;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
	const wchar_t window_class_name[] = L"ZMB_GAME";
	const WNDCLASS window_class =
	{
		.lpfnWndProc = WindowProc,
		.hInstance = hInstance,
		.lpszClassName = window_class_name
	};
	RegisterClass(&window_class);

	BitmapInfo.bmiHeader =
	{
		.biSize = sizeof(BitmapInfo.bmiHeader),
		.biPlanes = 1,
		.biBitCount = 32,
		.biCompression = BI_RGB
	};

	DeviceContextHandle = CreateCompatibleDC(NULL);

	// Create the window.
	HWND window_handle = CreateWindow(window_class_name, WindowTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		640, 360, 640, 360, NULL, NULL, hInstance, NULL);

	// Failed to create the window.
	if (window_handle == NULL)
	{
		return -1;
	}

	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	std::cout << "Console Enabled \n";

	// Gameloop.
	while (!quit) {
		MSG message = {};

		// Handle incoming messages.
		if (PeekMessage(&message, window_handle, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}

		Game::Update();
		Game::Render();

		InvalidateRect(window_handle, NULL, FALSE);
		UpdateWindow(window_handle);

		// 60 FPS.
		Sleep(16);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND window_handle, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		case WM_QUIT:
		{
			quit = true;

			break;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT paint;
			HDC device_context_handle = BeginPaint(window_handle, &paint);

			RECT rect = paint.rcPaint;
			int width = rect.right - rect.left;
			int height = rect.bottom - rect.top;

			int halfWidth = Frame.Width / 2;

			int segments = 512;
			int insanity = 8;
			int quality = Frame.Width / segments;

			// HACK.
			DrawRect(0, 0, Frame.Width, Frame.Height, 0x000000);
			for (int x = 0; x < segments; x++)
			{
				int rnd = rand() % insanity;

				// ( Position ) - ( ( Half Segments ) * Width ) + ( Width / 2 ) 
				DrawRect(((x * quality) - ((segments / 2) * quality) + (quality / 2)), 0, quality, Frame.Height / 2 + rnd, 0xFFFF00);
			}

			BitBlt(device_context_handle,   // Destination.
				rect.left, rect.top,        // Upper-left corner.
				width, height,              // Rect size.
				DeviceContextHandle,        // Source.
				rect.left, rect.top,        // Upper-left corner.
				SRCCOPY);                   // Raster-operation.

			EndPaint(window_handle, &paint);

			break;
		}

		case WM_SIZE:
		{
			// Dispose of the previous bitmap.
			if (Bitmap != NULL)
			{
				DeleteObject(Bitmap);
			}

			int width = LOWORD(lParam);
			int height = HIWORD(lParam);

			BitmapInfo.bmiHeader.biWidth = width;
			BitmapInfo.bmiHeader.biHeight = height;

			// Create a new bitmap that fits the window.
			Bitmap = CreateDIBSection(DeviceContextHandle, &BitmapInfo, DIB_RGB_COLORS, (void**)&Frame.Pixels, NULL, 0);
			if (Bitmap == NULL)
			{
				return -1;
			}

			// Tell the window to render the bitmap.
			SelectObject(DeviceContextHandle, Bitmap);

			Frame.Width = width;
			Frame.Height = height;

			break;
		}

		default:
		{
			return DefWindowProc(window_handle, message, wParam, lParam);
		}
	}

	return 0;
}