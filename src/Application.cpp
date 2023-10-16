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

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif

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
		640, 300, 640, 480, NULL, NULL, hInstance, NULL);

	// Failed to create the window.
	if (window_handle == NULL)
	{
		return -1;
	}

	// Gameloop.
	while (!quit) {
		MSG message = {};

		// Handle incoming messages.
		while (PeekMessage(&message, window_handle, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}


		for (int i = 0; i < Frame.Width * Frame.Height; i++)
		{
			Frame.Pixels[i % (Frame.Width * Frame.Height)] = Rand32();
		}

		InvalidateRect(window_handle, NULL, FALSE);
		UpdateWindow(window_handle);
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