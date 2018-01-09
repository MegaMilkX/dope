#include "../../include/core.h"
#include <windows.h>
#define STB_IMAGE_IMPLEMENTATION
extern "C" {
#include "stb_image.h"
}

#include "gl/glextutil.h"
#include "framebuffer.h"
#include "shader_program.h"
#include "mesh.h"

#include <iostream>

#include "gfxm.h"

int tbpp = 0;
int twidth = 0;
int theight = 0;
unsigned char* texData = 0;

HWND hWnd;
HDC deviceContext;
HGLRC context;
HGLRC threadingContext;
int contextVersion = 0;

bool InitGL()
{
  // == Hidden window ========
  HINSTANCE hInstance = GetModuleHandle(0);
  
  WNDCLASSEX wc;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = 0;
  wc.lpfnWndProc = DefWindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = L"window";
  wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  
  if(!RegisterClassEx(&wc))
  {
    return false;
  }
  
  hWnd = CreateWindowExW(
    0,
    L"window",
    L"hiddenwindow",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
    NULL, NULL, hInstance, NULL
  );
  
  ShowWindow(hWnd, SW_HIDE);
  //ShowWindow(hWnd, SW_SHOW);
  UpdateWindow(hWnd);
  
  // == OpenGL ===============
  PIXELFORMATDESCRIPTOR pfd = { 0 };
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cDepthBits = 32;
  pfd.iLayerType = PFD_MAIN_PLANE;
  
  deviceContext = GetDC(hWnd);
  
  int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
  
  if(!pixelFormat)
    return false;
  
  if(!SetPixelFormat(deviceContext, pixelFormat, &pfd))
    return false;
  
  HGLRC renderingContext = wglCreateContext(deviceContext);
  wglMakeCurrent(deviceContext, renderingContext);
  
  bool core_profile = false;
  WGLEXTLoadFunctions();
  if(!wglCreateContextAttribsARB)
  {
      context = renderingContext;
      threadingContext = wglCreateContext(deviceContext);
      wglMakeCurrent(NULL, NULL);
      wglShareLists(threadingContext, context);
      wglMakeCurrent(deviceContext, context);
  }
  else
  {
      int attr[] =
      {
          WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
          WGL_CONTEXT_MINOR_VERSION_ARB, 5,
          WGL_CONTEXT_FLAGS_ARB, 0,
          0
      };
      
      HGLRC renderingContext3plus = wglCreateContextAttribsARB(deviceContext, 0, attr);
      
      if(!renderingContext3plus)
      {
          context = renderingContext;
          threadingContext = wglCreateContext(deviceContext);
          wglMakeCurrent(NULL, NULL);
          wglShareLists(threadingContext, context);
          wglMakeCurrent(deviceContext, context);
      }
      else
      {
          context = renderingContext3plus;
          threadingContext = wglCreateContextAttribsARB(deviceContext, context, attr);
          wglMakeCurrent(NULL,NULL);
          wglDeleteContext(renderingContext);
          wglMakeCurrent(deviceContext, context);
          core_profile = true;
      }
  }
  
  GLEXTLoadFunctions();

  if (core_profile)
  {
      //For newer context at least one vao is required
      GLuint vao;
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);
  }
  
  int version[2];
  glGetIntegerv(GL_MAJOR_VERSION, &version[0]);
  glGetIntegerv(GL_MINOR_VERSION, &version[1]);
  contextVersion = version[0] * 100 + version[1] * 10;
  
  std::cout << "OpenGL v" << contextVersion << " ready.\n";
  
  std::cout << "GLSL v" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
  
  //=======================================================
  glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
  
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  wglSwapIntervalEXT(0);
  
  return true;
}

FrameBuffer* fb;
Texture2D* colorTexture;
ShaderProgram* shaderProgram;
MeshBuffer* cube;
Texture2D* texture;

void RenderInit()
{
  stbi_set_flip_vertically_on_load(1);
  texData = stbi_load("test.png", &twidth, &theight, &tbpp, 4);
  
  InitGL();
  
  colorTexture = new Texture2D();
  fb = new FrameBuffer(1280, 720);
  fb->SetTexture(0, colorTexture);
  
  shaderProgram = new ShaderProgram();
  shaderProgram->VertexShader(
#include "vs.glsl"
  );
  shaderProgram->FragmentShader(
#include "fs.glsl"
  );
  shaderProgram->Link();
  
  std::vector<gfxm::vec3> vertexData = {
    { -0.5f, -0.5f, 0.5f },
    { 0.5f, -0.5f, 0.5f },
    { 0.5f, -0.5f, -0.5f },
    { -0.5f, -0.5f, -0.5f },
    { -0.5f,  0.5f, 0.5f },
    { 0.5f,  0.5f, 0.5f },
    { 0.5f,  0.5f, -0.5f },
    { -0.5f,  0.5f, -0.5f }
  };

  std::vector<gfxm::vec2> uvData = {
    { 0.0f, 0.0f },
    { 1.0f, 0.0f },
    { 0.0f, 1.0f },
    { 1.0f, 1.0f },
    { 0.0f, 1.0f },
    { 1.0f, 1.0f },
    { 0.0f, 0.0f },
    { 1.0f, 0.0f }
  };

  std::vector<unsigned short> indexData = {
    0, 2, 1, 0, 3, 2,
    4, 5, 6, 4, 6, 7,
    0, 5, 4, 0, 1, 5,
    2, 3, 7, 2, 7, 6,
    0, 7, 3, 0, 4, 7,
    1, 6, 5, 1, 2, 6
  };

  cube = new MeshBuffer();
  cube->SetPositionData(vertexData);
  cube->SetUVData(uvData);
  cube->SetIndexData(indexData);
  
  texture = new Texture2D();
  texture->Upload(texData, twidth, theight, tbpp);
}

gfxm::transform model;

void RenderUpdate(ScreenBuffer* screen)
{
  glViewport(0,0,1280,720);
  //glBindFramebuffer(GL_FRAMEBUFFER, 0);
  fb->Bind();
  glClearColor(0.2f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  gfxm::mat4 projection;
  projection = gfxm::perspective(projection, 1.4f, 16.0f/9.0f, 0.1f, 100.0f);
  gfxm::transform view;
  view.translate(0.0f, 0.7f, 2.0f);
  model.rotate(0.1f, gfxm::vec3(0.0f, 1.0f, 0.0f));
  
  shaderProgram->Bind();
  glUniformMatrix4fv(shaderProgram->UniformLocation("projection"), 1, GL_FALSE, (float*)&projection);
  glUniformMatrix4fv(shaderProgram->UniformLocation("view"), 1, GL_FALSE, (float*)&gfxm::inverse(view.matrix()));
  glUniformMatrix4fv(shaderProgram->UniformLocation("model"), 1, GL_FALSE, (float*)&model.matrix());
  texture->Bind(0);
  cube->Bind();
  cube->Render();
  
  colorTexture->Bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, screen->data);
  
  //SwapBuffers(deviceContext);
}

void RenderCleanup()
{
  stbi_image_free(texData);
}