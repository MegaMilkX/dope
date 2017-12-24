#include <stdint.h>
#include <windows.h>
#include "screen_buffer.h"
#include <stdio.h>

struct RenderModule
{
  typedef void(*FuncInit_t)(void) ;
  typedef void(*FuncUpdate_t)(ScreenBuffer*) ;
  typedef void(*FuncCleanup_t)(void) ;
  
  FuncInit_t Init;
  FuncUpdate_t Update;
  FuncCleanup_t Cleanup;
  
  void Load()
  {
    HMODULE module = LoadLibraryA("render_module.dll");
    Init = (FuncInit_t)GetProcAddress(module, "RenderInit");
    Update = (FuncUpdate_t)GetProcAddress(module, "RenderUpdate");
    Cleanup = (FuncCleanup_t)GetProcAddress(module, "RenderCleanup");
  }
};

RenderModule renderModule;

HWND hWnd = 0;
HDC hdc;
MSG msg;
int width = 1280;
int height = 720;
int bpp = 4;

ScreenBuffer screen;
BITMAPINFO bitsInfo;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  LRESULT result = 0;
  switch(msg)
  {
  case WM_CLOSE:
    DestroyWindow(hWnd);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }
  
  return result;
}

int InitWindow()
{
  HINSTANCE hInst = GetModuleHandle(0);
  WNDCLASSEX wc;
  
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInst;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = L"CoreWindow";
  wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  
  if(!RegisterClassEx(&wc))
  {
    // TODO: LOG RegisterClassEx failed
    return 1;
  }
  
  RECT rect;
  rect.left = 0;
  rect.top = 0;
  rect.right = width;
  rect.bottom = height;
  
  AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, 0);
  
  hWnd = CreateWindowExW(
    0,
    L"CoreWindow",
    L"Core Window",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    rect.right - rect.left,
    rect.bottom - rect.top,
    NULL,
    NULL,
    hInst,
    NULL
  );
  
  ShowWindow(hWnd, 1);
  UpdateWindow(hWnd);
  
  hdc = GetDC(hWnd);
  PIXELFORMATDESCRIPTOR pfd = { 0 };
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 32;
  pfd.iLayerType = PFD_MAIN_PLANE;
  int pixelFmt = ChoosePixelFormat(hdc, &pfd);
  if(!pixelFmt)
  {
    // TODO: ChoosePixelFormat failed
    return 1;
  }
  
  if(!SetPixelFormat(hdc, pixelFmt, &pfd))
  {
    // TODO: SetPixelFormat failed
    return 1;
  }
  
  return 0;
}

int InitScreen()
{
  bpp = 4;
  screen.data = new unsigned char[width * height * bpp];
  screen.width = width;
  screen.height = height;
  screen.bpp = bpp;
  
  bitsInfo = { 0 };
  bitsInfo.bmiHeader.biSize = sizeof(bitsInfo.bmiHeader);
  bitsInfo.bmiHeader.biWidth = width;
  bitsInfo.bmiHeader.biHeight = height;
  bitsInfo.bmiHeader.biPlanes = 1;
  bitsInfo.bmiHeader.biBitCount = 8 * bpp;
  bitsInfo.bmiHeader.biCompression = BI_RGB;
  
  return 0;
}

WAVEFORMATEX wf;
WAVEHDR whdr;
HWAVEOUT hWaveOut;
LPSTR lpData;
float clpData[65536];

char buffer[44100 * 60];

int main()
{
  InitWindow();
  InitScreen();
  
  renderModule.Load();
  renderModule.Init();
  
  HWAVEOUT hWaveOut = 0;
	WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 44100, 44100, 1, 8, 0 };
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
	
	// See http://goo.gl/hQdTi
	for (DWORD t = 0; t < sizeof(buffer); ++t)
		buffer[t] = static_cast<char>((((t * (t >> 8 | t >> 9) & 46 & t >> 8)) ^ (t & t >> 13 | t >> 6)) & 0xFF);
	
  WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
  
  waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
	waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
  waveOutClose(hWaveOut);
  
  while(msg.message != WM_QUIT)
  {
    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
      if(msg.message == WM_QUIT)
        break;
    }
    
    renderModule.Update(&screen);
    
    StretchDIBits(
      hdc,
      0,
      0,
      width,
      height,
      0,
      0,
      width,
      height,
      (void*)screen.data,
      &bitsInfo,
      DIB_RGB_COLORS,
      SRCCOPY
    );
  }
  
  renderModule.Cleanup();
  
  return 0;
}