#ifndef CORE_IMPL_H
#define CORE_IMPL_H

#include <iostream>
#include <string>

#include "core_interface.h"
#include "scene_object_impl.h"

#include <windows.h>
#include <stdint.h>

#include <mmdeviceapi.h>
#include <Audioclient.h>
typedef LONGLONG REFERENCE_TIME;

std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}
  
class Timer
{
public:
    Timer()
    {
        QueryPerformanceFrequency(&freq);
    }

    void Start()
    {
        QueryPerformanceCounter(&start);
    }
    
    int64_t End()
    {
        QueryPerformanceCounter(&end);
        elapsed.QuadPart = end.QuadPart - start.QuadPart;
        elapsed.QuadPart *= 1000000;
        elapsed.QuadPart /= freq.QuadPart;
        return elapsed.QuadPart;
    }
private:
    LARGE_INTEGER freq;
    LARGE_INTEGER start, end;
    LARGE_INTEGER elapsed;
};

inline LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

Component* CreateComponentStub(SceneObject* s, const char* type)
{
  return 0;
}

struct RenderModule
{
  typedef void(*FuncInit_t)(void) ;
  typedef void(*FuncUpdate_t)(ScreenBuffer*) ;
  typedef void(*FuncCleanup_t)(void) ;
  typedef Component*(*FuncCreateComponent_t)(SceneObject*, const char*);
  
  FuncInit_t Init;
  FuncUpdate_t Update;
  FuncCleanup_t Cleanup;
  FuncCreateComponent_t CreateComponent;
  
  void Load()
  {
    HMODULE module = LoadLibraryA("render.dll");
    Init = (FuncInit_t)GetProcAddress(module, "RenderInit");
    Update = (FuncUpdate_t)GetProcAddress(module, "RenderUpdate");
    Cleanup = (FuncCleanup_t)GetProcAddress(module, "RenderCleanup");
    CreateComponent = (FuncCreateComponent_t)GetProcAddress(module, "CreateComponent");
    if(!CreateComponent)
      CreateComponent = &CreateComponentStub;
  }
};

struct AudioModule
{
  typedef void(*FuncInit_t)(void);
  typedef void(*FuncUpdate_t)(void*, int, int);
  typedef void(*FuncCleanup_t)(void);
  typedef Component*(*FuncCreateComponent_t)(SceneObject*, const char*);
  
  FuncInit_t Init;
  FuncUpdate_t Update;
  FuncCleanup_t Cleanup;
  FuncCreateComponent_t CreateComponent;
  
  void Load()
  {
    HMODULE module = LoadLibraryA("audio.dll");
    Init = (FuncInit_t)GetProcAddress(module, "AudioInit");
    Update = (FuncUpdate_t)GetProcAddress(module, "AudioUpdate");
    Cleanup = (FuncCleanup_t)GetProcAddress(module, "AudioCleanup");
    CreateComponent = (FuncCreateComponent_t)GetProcAddress(module, "CreateComponent");
    if(!CreateComponent)
      CreateComponent = &CreateComponentStub;
  }
};

struct GameModule
{
  typedef void(*FuncInit_t)(CoreInterface* coreInterface);
  typedef void(*FuncUpdate_t)(void);
  typedef void(*FuncCleanup_t)(void);
  
  FuncInit_t Init;
  FuncUpdate_t Update;
  FuncCleanup_t Cleanup;
  
  void Load()
  {
    HMODULE module = LoadLibraryA("game.dll");
    Init = (FuncInit_t)GetProcAddress(module, "GameInit");
    Update = (FuncUpdate_t)GetProcAddress(module, "GameUpdate");
    Cleanup = (FuncCleanup_t)GetProcAddress(module, "GameCleanup");
  }
};

class Core : public CoreInterface
{
public:
    Core()
    : dt(0.0f) {}
    
    bool Init()
    {
        InitWindow();
        InitScreen();
      
        renderModule.Load();
        renderModule.Init();
        audioModule.Load();
        audioModule.Init();
        gameModule.Load();
        gameModule.Init(this);
        
        WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 2, 44100, 44100, 4, 16, 0 };
        
        /*
        waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
        waveOutSetVolume(hWaveOut, (DWORD)0x80008000UL);
        // See http://goo.gl/hQdTi
        
        
        header = { (char*)buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
        
        waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
        waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
        */
        HRESULT hr;
        REFERENCE_TIME hnsRequestedDuration = 0;
        IMMDeviceEnumerator *pEnumerator = NULL;
        IMMDevice *pDevice = NULL;
        IAudioClient *pAudioClient = NULL;
        IAudioRenderClient *pRenderClient = NULL;
        HANDLE hEvent = NULL;
        HANDLE hTask = NULL;
        UINT32 bufferFrameCount;
        BYTE *pData;
        DWORD flags = 0;
        const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
        const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
        const IID IID_IAudioClient = __uuidof(IAudioClient);
        const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
        hr = CoCreateInstance(
            CLSID_MMDeviceEnumerator,
            0,
            CLSCTX_ALL,
            IID_IMMDeviceEnumerator,
            (void**)&pEnumerator
        );
        if(FAILED(hr))
        {
            std::cout << "CoCreateInstance failed" << std::endl;
        }
        
        hr = pEnumerator->GetDefaultAudioEndpoint(
            eRender,
            eConsole,
            &pDevice
        );
        if(FAILED(hr))
        {
            std::cout << "GetDefaultAudioEndpoint failed" << std::endl;
        }
        
        hr = pDevice->Activate(
            IID_IAudioClient,
            CLSCTX_ALL,
            NULL,
            (void**)&pAudioClient
        );
        if(FAILED(hr))
        {
            std::cout << "Activate failed" << std::endl;
        }
        
        hr = pAudioClient->GetDevicePeriod(NULL, &hnsRequestedDuration);
        if(FAILED(hr))
        {
            std::cout << "GetDevicePeriod failed" << std::endl;
        }
        std::cout << "GetDevicePeriod: " << hnsRequestedDuration << std::endl;
        
        hr = pAudioClient->Initialize(
            AUDCLNT_SHAREMODE_SHARED,
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
            hnsRequestedDuration,
            0,
            &wfx,
            NULL
        );
        if(FAILED(hr))
        {
            std::cout << "audio client Initialize failed: " << std::hex << hr << " " << GetLastErrorAsString() << std::endl;
        }
        
        hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        
        hr = pAudioClient->SetEventHandle(hEvent);
        if(FAILED(hr))
        {
            std::cout << "SetEventHandle failed" << std::endl;
        }
        
        hr = pAudioClient->GetBufferSize(&bufferFrameCount);
        if(FAILED(hr))
        {
            std::cout << "GetBufferSize failed" << std::endl;
        }
        
        hr = pAudioClient->GetService(
            IID_IAudioRenderClient,
            (void**)&pRenderClient
        );
        if(FAILED(hr))
        {
            std::cout << "GetService failed" << std::endl;
        }
        
        pAudioClient->Start();
        if(FAILED(hr))
        {
            std::cout << "Start failed" << std::endl;
        }
        
        hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
        if(FAILED(hr))
        {
            std::cout << "GetBuffer failed" << std::endl;
        }
        audioModule.Update((void*)pData, 44100, 0);
        hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
        if(FAILED(hr))
        {
            std::cout << "ReleaseBuffer failed" << std::endl;
        }
      
        return true;
    }
    
    void Cleanup()
    {
        waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
        waveOutClose(hWaveOut);
      
        gameModule.Cleanup();
        audioModule.Cleanup();
        renderModule.Cleanup();
    }
    
    void Run()
    {
        while(msg.message != WM_QUIT)
        {
            timer.Start();
            while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
                if(msg.message == WM_QUIT)
                    break;
            }
            
            gameModule.Update();
            
            renderModule.Update(&screen);

            SetDIBitsToDevice(
                hdc,
                0,
                0,
                width,
                height,
                0,
                0,
                0,
                height,
                (void*)screen.data,
                &bitsInfo,
                DIB_RGB_COLORS
            );
            
            dt = timer.End() / 1000000.0f;
            /*
            audioModule.Update((void*)buffer, 44100, 44100 * dt);
            header = { (char*)buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
            waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
            waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
            */
        }
    }

    virtual SceneObject* CreateScene()
    {
        return new SceneObjectImpl(this);
    }
    
    virtual void DestroyScene(SceneObject* so)
    {
        delete so;
    }
    
    virtual Component* CreateComponent(SceneObject* so, const char* type)
    {
        // TODO 
        return 0;
    }
    
    virtual void DestroyComponent(Component* ptr, const char* type)
    {
        
    }
private:
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
      pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
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

    HWND hWnd = 0;
    HDC hdc;
    MSG msg;
    int width = 1280;
    int height = 720;
    int bpp = 4;

    ScreenBuffer screen;
    BITMAPINFO bitsInfo;

    WAVEFORMATEX wf;
    WAVEHDR whdr;
    HWAVEOUT hWaveOut;
    WAVEHDR header;
    LPSTR lpData;
    float clpData[65536];

    short buffer[44100];
    
    RenderModule renderModule;
    AudioModule audioModule;
    GameModule gameModule;
    
    Timer timer;
    float dt;
};

#endif
