// TODO: Abstract uncomprehensible win32 bullshittery.

#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>

#include "Log.h"

const wchar_t WindowName[] = L"Zombie Game";
const wchar_t WindowClassName[] = L"ZMB_GAME";

static bool quit = false;

struct
{
	int Width;
	int Height;
	uint32_t* Pixels;
} Frame;

void DrawRect(int x, int y, int width, int height, int hexColor)
{
	// Convert coordinate space to pixel space.
	x += (Frame.Width * 0.5f) - (width * 0.5f);
	y += (Frame.Height * 0.5f) - (height * 0.5f);

	for (int yi = y; yi < y + height; yi++)
	{
		for (int xi = x; xi < x + width; xi++)
		{
			Frame.Pixels[(Frame.Width * yi) + xi] = hexColor;
		}
	}
}

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

static BITMAPINFO BitmapInfo;
static HBITMAP Bitmap;
static HDC DeviceContextHandle;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PSTR lpCmdLine, _In_ int nCmdShow)
{
#ifdef _DEBUG
	// Create a console window.
	AllocConsole();

	// Redirect output stream.
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);

	Log::Info("Console Initialized");
#endif

	const WNDCLASS window_class =
	{
		.lpfnWndProc = WindowProc,
		.hInstance = hInstance,
		.lpszClassName = WindowClassName
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

	// Create the game window.
	HWND window_handle = CreateWindow(
		WindowClassName, WindowName,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		640, 360, 640, 360,
		NULL, NULL, hInstance, NULL);

	if (window_handle == NULL)
	{
		Log::Error("Failed to create game window");
		return -1;
	}

	// Gameloop.
	while (!quit) {
		MSG message = {};

		// Handle incoming messages.
		if (PeekMessage(&message, window_handle, 0, 0, PM_REMOVE))
		{
			DispatchMessage(&message);
		}

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
			Log::Info("Shutting down");

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

			float segments = Frame.Width;
			int insanity = 100;
			int quality = Frame.Width / segments;

			float halfSegments = segments / 2;

			// Clear pixels.
			DrawRect(0, 0, Frame.Width, Frame.Height, 0x000000);

			for (int x = -halfSegments; x <= halfSegments; x++)
			{
				int rnd = (rand() % insanity);

				DrawRect(x * quality, 0, quality, Frame.Height / 2 + rnd, 0xFFFF00);
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