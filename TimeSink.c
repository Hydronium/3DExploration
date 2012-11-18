#include <windows.h>
#include <sys/stat.h>
#include "glcorearb.h"
#include "fnld.h"
 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
 
HGLRC       hRC         = NULL;
HDC         hDC         = NULL;
HWND        hWnd        = NULL;
HINSTANCE   hInstance   = NULL;
 
bool      keys[256];
bool      active        = TRUE;
bool      fullscreen    = TRUE;

unsigned int errort;
GLuint myProgram;
GLuint positionBufferObject;

float screen_width = 0.0, screen_height = 0.0; 

/*----------------------------------------------*/
LRESULT CALLBACK WndProc(     HWND hWnd,
                              UINT uMSG,
                              WPARAM wParam,
                              LPARAM lParam);
 
void FnLdInit(void);

GLvoid ResizeGLScene(GLsizei width, GLsizei height);

GLvoid InitGL(GLvoid);

int DrawGLScene(GLvoid);

GLvoid KillGLWindow(GLvoid);

bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag);

int WINAPI WinMain(     HINSTANCE hInstance,
                        HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine,
                        int nCmdShow);
/*----------------------------------------------*/

void FnLdInit(void)
{
      HINSTANCE hGLLIB = NULL;
      hGLLIB = LoadLibrary("opengl32.dll");
      
      glActiveTexture                     = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
      glAttachShader                      = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
      glBindBuffer                        = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
      glBufferData                        = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
      glCompileShader                     = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
      glCreateProgram                     = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
      glCreateShader                      = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
      glDeleteShader                      = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
      glDetachShader                      = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
      glDisableVertexAttribArray          = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
      glEnableVertexAttribArray           = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
      glGenBuffers                        = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
      glGetShaderInfoLog                  = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
      glGetShaderiv                       = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
      glGetUniformfv                      = (PFNGLGETUNIFORMFVPROC)wglGetProcAddress("glGetUniformfv");
      glGetUniformLocation                = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
      glLinkProgram                       = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
      glShaderSource                      = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
      glUniform1i                         = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
      glUniform2f                         = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
      glUseProgram                        = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
      glVertexAttribPointer               = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
      
      glBindTexture           = (PFNGLBINDTEXTUREPROC)GetProcAddress(hGLLIB, "glBindTexture");
      glClear                 = (PFNGLCLEARPROC)GetProcAddress(hGLLIB, "glClear");
      glClearColor            = (PFNGLCLEARCOLORPROC)GetProcAddress(hGLLIB, "glClearColor");
      glClearDepth            = (PFNGLCLEARDEPTHPROC)GetProcAddress(hGLLIB, "glClearDepth");
      glDepthFunc             = (PFNGLDEPTHFUNCPROC)GetProcAddress(hGLLIB, "glDepthFunc");
      glDrawArrays            = (PFNGLDRAWARRAYSPROC)GetProcAddress(hGLLIB, "glDrawArrays");
      glEnable                = (PFNGLENABLEPROC)GetProcAddress(hGLLIB, "glEnable");
      glGenTextures           = (PFNGLGENTEXTURESPROC)GetProcAddress(hGLLIB, "glGenTextures");
      glTexImage2D            = (PFNGLTEXIMAGE2DPROC)GetProcAddress(hGLLIB, "glTexImage2D");
      glTexParameteri         = (PFNGLTEXPARAMETERIPROC)GetProcAddress(hGLLIB, "glTexParameteri");
      glViewport              = (PFNGLVIEWPORTPROC)GetProcAddress(hGLLIB, "glViewport");
      glDrawElements          = (PFNGLDRAWELEMENTSPROC)GetProcAddress(hGLLIB, "glDrawElements");
      glGetError              = (PFNGLGETERRORPROC)GetProcAddress(hGLLIB, "glGetError");
      glGetString             = (PFNGLGETSTRINGPROC)GetProcAddress(hGLLIB, "glGetString");
}

 
GLvoid InitGL(GLvoid)
{
      //blah();
      FnLdInit();

      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClearDepth(1.0f);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LEQUAL);
}

bool CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
      GLuint PixelFormat;
      WNDCLASS wc;
      DWORD dwExStyle;
      DWORD dwStyle;

      RECT WindowRect;
      WindowRect.left = (long)0;
      WindowRect.right = (long)width;
      WindowRect.top = (long)0;
      WindowRect.bottom = (long)height;

      fullscreen = fullscreenflag;

      hInstance = GetModuleHandle(NULL);

      wc.style = CS_HREDRAW |CS_VREDRAW | CS_OWNDC;
      wc.lpfnWndProc = (WNDPROC) WndProc;
      wc.cbClsExtra = 0;
      wc.cbWndExtra = 0;
      wc.hInstance = hInstance;
      wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
      wc.hCursor = LoadCursor(NULL, IDC_ARROW);
      wc.hbrBackground = NULL;
      wc.lpszMenuName = NULL;
      wc.lpszClassName = "OpenGL";

      if (!RegisterClass(&wc))
      {
            MessageBox(NULL, "Failed to register the window class", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      if (fullscreen)
      {
            int changeresult;
            //char msg[100];
            DISPLAY_DEVICE disp;
            DEVMODE dmScreenSettings;
            int dw;

            memset(&disp, 0, sizeof(disp));
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

            disp.cb = sizeof(disp);


            if (!(EnumDisplayDevices(NULL, 0, &disp, 0)))
            {
                  return 1;
            }

             
            dmScreenSettings.dmSize = sizeof(dmScreenSettings);

            //ENUM_CURRENT_SETTINGS
            if (!(EnumDisplaySettings(disp.DeviceName, ENUM_CURRENT_SETTINGS, &dmScreenSettings)))
            {
                  dw = GetLastError();
            }

            dmScreenSettings.dmPelsWidth = width;
            dmScreenSettings.dmPelsHeight = height;

            changeresult = ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

            //dmScreenSettings.dmPelsWidth = width;
            //dmScreenSettings.dmPelsHeight = height;
            //dmScreenSettings.dmBitsPerPel = bits;
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

            if (changeresult != DISP_CHANGE_SUCCESSFUL)
            {
                  if (MessageBox(NULL, "Use window mode?", "GL", MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
                  {
                        fullscreen = FALSE;
                  }
                  else
                  {
                        return FALSE;
                  }
            }
      }

      if (fullscreen)
      {
            dwExStyle = WS_EX_APPWINDOW;
            dwStyle = WS_POPUP;
            ShowCursor(FALSE);
      }
      else
      {
            dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
            dwStyle = WS_OVERLAPPEDWINDOW;
      }

      AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

      if(!(hWnd = CreateWindowEx(   dwExStyle,
                                    "OpenGL",
                                    title,
                                    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | dwStyle,
                                    0,
                                    0,
                                    WindowRect.right - WindowRect.left,
                                    WindowRect.bottom - WindowRect.top,
                                    NULL,
                                    NULL,
                                    hInstance,
                                    NULL)))
      {
            KillGLWindow();
            MessageBox(NULL, "Window Creation error", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      static  PIXELFORMATDESCRIPTOR pfd=                  // pfd Tells Windows How We Want Things To Be
      {
          sizeof(PIXELFORMATDESCRIPTOR),                  // Size Of This Pixel Format Descriptor
          1,                              // Version Number
          PFD_DRAW_TO_WINDOW |            // Format Must Support Window
          PFD_SUPPORT_OPENGL |            // Format Must Support OpenGL
          PFD_DOUBLEBUFFER,               // Must Support Double Buffering
          PFD_TYPE_RGBA,                  // Request An RGBA Format
          0,                           // Select Our Color Depth
          0, 0, 0, 0, 0, 0,               // Color Bits Ignored
          0,                              // No Alpha Buffer
          0,                              // Shift Bit Ignored
          0,                              // No Accumulation Buffer
          0, 0, 0, 0,                     // Accumulation Bits Ignored
          24,                             // 16Bit Z-Buffer (Depth Buffer)
          0,                              // No Stencil Buffer
          0,                              // No Auxiliary Buffer
          PFD_MAIN_PLANE,                 // Main Drawing Layer
          0,                              // Reserved
          0, 0, 0                         // Layer Masks Ignored
      };
      pfd.cColorBits = bits;

      if (!(hDC = GetDC(hWnd)))
      {
            KillGLWindow();
            MessageBox(NULL, "Can't create a GL device context", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
      {
            KillGLWindow();
            MessageBox(NULL, "Can't find a suitable pixelformat", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      if (!SetPixelFormat(hDC, PixelFormat, &pfd))
      {
            KillGLWindow();
            MessageBox(NULL, "Can't set the pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      if (!(hRC = wglCreateContext(hDC)))
      {
            KillGLWindow();
            MessageBox(NULL, "Can't create a GL rendering context", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      if (!wglMakeCurrent(hDC, hRC))
      {
            KillGLWindow();
            MessageBox(NULL, "Can't activate the GL rendering context", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      ShowWindow(hWnd, SW_SHOW);
      SetForegroundWindow(hWnd);
      SetFocus(hWnd);

      InitGL();
      ResizeGLScene(width, height);

      return TRUE;
}

GLvoid KillGLWindow(GLvoid)
{
      if (fullscreen)
      {
            ChangeDisplaySettings(NULL, 0);
            ShowCursor(TRUE);
      }

      if (hRC)
      {
            if (!wglMakeCurrent(NULL, NULL))
            {
                  MessageBox(NULL, "Release of DC and RC failed.","Error", MB_OK | MB_ICONINFORMATION);
            }

            if (!wglDeleteContext(hRC))
            {
                  MessageBox(NULL, "Release of Rendering context failed", "Error", MB_OK | MB_ICONINFORMATION);
            }
            hRC = NULL;

            if (hWnd && !DestroyWindow(hWnd))
            {
                  MessageBox(NULL, "Could not release hWnd", "Error", MB_OK | MB_ICONINFORMATION);
                  hWnd = NULL;
            }

            if (!UnregisterClass("OpenGL", hInstance))
            {
                  MessageBox(NULL, "Could not unregister the class", "Error", MB_OK | MB_ICONINFORMATION);
                  hInstance = NULL;
            }
      }
}

int DrawGLScene(GLvoid)
{
      //float uniforms[2];
      /*
      int textureLocation;

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
      glUseProgram(myProgram);
 
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, positionBufferObject);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)(3200*4));

      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);


      glActiveTexture(GL_TEXTURE0);
      //textureLocation = glGetUniformLocation(myProgram, "textureS2D");
      glUniform1i(/*textureLocationglGetUniformLocation(myProgram, "textureS2D"), 0);
      glUniform2f(glGetUniformLocation(myProgram, "screenDim"), screen_width, screen_height);

      glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_BYTE, indices);

      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
      glUseProgram(0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
 
      return TRUE;
}

GLvoid ResizeGLScene(GLsizei width, GLsizei height)
{
      if (height == 0)
      {
            height = 1;
      }
      
      glViewport(0, 0, width, height);
      screen_width = width;
      screen_height = height;
}

LRESULT CALLBACK WndProc(     HWND hWnd,
                              UINT uMSG,
                              WPARAM wParam,
                              LPARAM lParam)
{
      switch(uMSG)
      {
            case WM_ACTIVATE:
            {
                  if (!HIWORD(wParam))
                  {
                        active = TRUE;
                  }
                  else
                  {
                        active = FALSE;
                  }

                  return 0;
            }
            case WM_SYSCOMMAND:
            {
                  switch(wParam)
                  {
                        case SC_SCREENSAVE:
                        case SC_MONITORPOWER:
                        return 0;
                  }
                  break;
            }
            case WM_CLOSE:
            {
                  PostQuitMessage(0);
                  return 0;
            }
            case WM_KEYDOWN:
            {
                  keys[wParam] = TRUE;
                  return 0;
            }
            case WM_KEYUP:
            {
                  keys[wParam] = FALSE;
                  return 0;
            }
            case WM_SIZE:
            {
                  ResizeGLScene(LOWORD(lParam), HIWORD(lParam));

                  return 0;
            }
      }

      return DefWindowProc(hWnd, uMSG, wParam, lParam);
}
 
int WINAPI WinMain(     HINSTANCE hInstance,
                        HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine,
                        int nCmdShow)
{
      MSG msg;
      bool done = FALSE;
      screen_width = 640;
      screen_height = 480;

      if (MessageBox(NULL, "Fullscreen?", "Well?", MB_YESNO | MB_ICONQUESTION) == IDNO)
      {
            fullscreen = FALSE;
      }

      if (!CreateGLWindow("My window", (int)screen_width, (int)screen_height, 32, fullscreen))
      {
            return 0;
      }



      while (!done)
      {
            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                  if (msg.message == WM_QUIT)
                  {
                        done = TRUE;
                  }
                  else
                  {
                        TranslateMessage(&msg);
                        DispatchMessage(&msg);
                  }
            }
            else
            {
                  if (active)
                  {
                        if (keys[VK_ESCAPE])
                        {
                              done = TRUE;
                        }
                        else
                        {
                              DrawGLScene();
                              SwapBuffers(hDC);
                        }
                  }

                  if (keys[VK_F1])
                  {
                        keys[VK_F1] = FALSE;
                        KillGLWindow();
                        fullscreen = !fullscreen;

                        if (!CreateGLWindow("My window", screen_width, screen_height, 24, fullscreen))
                        {
                              return 0;
                        }
                  }
            }
      }

      KillGLWindow();
      return (msg.wParam);
}
