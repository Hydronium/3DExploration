#include <windows.h>
#include <sys/stat.h>

#include "glcorearb.h"
#include "fnld.h"
 
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
/*----------------------------------------------------*/
/*Variables*/
/*----------------------------------------------------*/
HGLRC       hRC         = NULL;
HDC         hDC         = NULL;

int globCmdShow;
ATOM resultRegisterClass;

int width, height;

bool fullscreen = FALSE;
/*----------------------------------------------------*/
/*Function declarations*/
/*----------------------------------------------------*/
int WINAPI WinMain(     HINSTANCE hInstance,
                        HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine,
                        int nCmdShow);

LRESULT CALLBACK WndProc(     HWND hWnd,
                              UINT uMSG,
                              WPARAM wParam,
                              LPARAM lParam);

bool GLSetup(HWND hWnd, LPARAM lParam);

/*----------------------------------------------------*/
/*Functions*/
/*----------------------------------------------------*/
int WINAPI WinMain(     HINSTANCE hInstance,
                        HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine,
                        int nCmdShow)
{
      bool done = FALSE;
      globCmdShow = nCmdShow;

      WNDCLASSEX wc;
      HWND hWnd;
      MSG Msg;
      DWORD dwExStyle;
      DWORD dwStyle;
      RECT WindowRect;

      width = 640;
      height = 480;

      wc.cbSize        = sizeof(WNDCLASSEX);
      wc.style         = CS_HREDRAW |CS_VREDRAW | CS_OWNDC;
      wc.lpfnWndProc   = WndProc;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = 0;
      wc.hInstance     = hInstance;
      wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
      wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
      wc.hbrBackground = NULL;//(HBRUSH)(COLOR_WINDOW+1);
      wc.lpszMenuName  = NULL;
      wc.lpszClassName = "TimeSink";
      wc.hIconSm       = LoadIcon(NULL, IDI_WINLOGO);

      resultRegisterClass = RegisterClassEx(&wc);
      if (!resultRegisterClass) // Attempt To Register The Window Class
      {
            return 0;
      }

      //Do we want fullscreen?
      if (fullscreen)
      {
            DISPLAY_DEVICE dispDevice;
            DEVMODE dmScreenSettings;
            int error;

            memset(&dispDevice, 0, sizeof(dispDevice));
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

            dispDevice.cb = sizeof(dispDevice);

            if (!(EnumDisplayDevices(NULL, 0, &dispDevice, 0)))
            {
                  error = GetLastError();
                  return 1;
            }
             
            dmScreenSettings.dmSize = sizeof(dmScreenSettings);

            if (!(EnumDisplaySettings(dispDevice.DeviceName, ENUM_CURRENT_SETTINGS, &dmScreenSettings)))
            {
                  error = GetLastError();
                  return 1;
            }

            dmScreenSettings.dmPelsWidth = width;
            dmScreenSettings.dmPelsHeight = height; 

            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

            if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
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

            //Fullscreen settings
            dwExStyle = WS_EX_APPWINDOW;
            dwStyle = WS_POPUP;
            ShowCursor(FALSE);
      }
      else
      {
            //Windowed settings
            dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
            dwStyle = WS_OVERLAPPEDWINDOW;
      }

      WindowRect.left = (long)0;
      WindowRect.right = (long)width;
      WindowRect.top = (long)0;
      WindowRect.bottom = (long)height;
      AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

      //Creating the window
      if(!(hWnd = CreateWindowEx(   dwExStyle, //Extended settings
                                    "TimeSink", //Class title
                                    "Test", //Window title
                                    WS_OVERLAPPEDWINDOW, //Regular settings
                                    0,//CW_USEDEFAULT, 
                                    0,//CW_USEDEFAULT, 
                                    WindowRect.right - WindowRect.left,
                                    WindowRect.bottom - WindowRect.top,
                                    NULL, 
                                    NULL, 
                                    hInstance, 
                                    NULL)))
      {
            return 1;
      }

      //ShowWindow(hWnd, nCmdShow);
      //ShowWindow(hWnd, SW_SHOW);
      //SetForegroundWindow(hWnd);
      //UpdateWindow(hWnd);

      while (!done)
      {
            if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE)) //Does not block.
            {
                  if (Msg.message == WM_QUIT)
                  {
                        done = TRUE;
                  }
                  else
                  {
                        TranslateMessage(&Msg);
                        DispatchMessage(&Msg);
                  }
            }
      }

      return Msg.wParam;
}

LRESULT CALLBACK WndProc(     HWND hWnd,
                              UINT uMSG,
                              WPARAM wParam,
                              LPARAM lParam)
{
      switch(uMSG)
      {
            case WM_CREATE:
            {
                  if (!(GLSetup(hWnd, lParam)))
                  {
                        return -1;
                  }
                  break;
            }
            case WM_CLOSE:
            {
                  DestroyWindow(hWnd);
                  break;
            }
            case WM_DESTROY:
            {
                  PostQuitMessage(0);
                  break;
            }
            default:
            {
                  return DefWindowProc(hWnd, uMSG, wParam, lParam);
            }
      }

      return 0;
}

bool GLSetup(HWND hWnd, LPARAM lParam)
{
      //CREATESTRUCT * pCreateStruct = (CREATESTRUCT *)lParam;
      GLuint PixelFormat;

      static  PIXELFORMATDESCRIPTOR pfd=  // pfd Tells Windows How We Want Things To Be
      {
          sizeof(PIXELFORMATDESCRIPTOR),  // Size Of This Pixel Format Descriptor
          1,                              // Version Number
          PFD_DRAW_TO_WINDOW |            // Format Must Support Window
          PFD_SUPPORT_OPENGL |            // Format Must Support OpenGL
          PFD_DOUBLEBUFFER,               // Must Support Double Buffering
          PFD_TYPE_RGBA,                  // Request An RGBA Format
          32,                             // Select Our Color Depth
          0, 0, 0, 0, 0, 0,               // Color Bits Ignored
          0,                              // No Alpha Buffer
          0,                              // Shift Bit Ignored
          0,                              // No Accumulation Buffer
          0, 0, 0, 0,                     // Accumulation Bits Ignored
          24,                             // Z-Buffer (Depth Buffer)
          0,                              // No Stencil Buffer
          0,                              // No Auxiliary Buffer
          PFD_MAIN_PLANE,                 // Main Drawing Layer
          0,                              // Reserved
          0, 0, 0                         // Layer Masks Ignored
      };

      if (!(hDC = GetDC(hWnd)))
      {
            MessageBox(NULL, "Can't create a GL device context", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd)))
      {
            MessageBox(NULL, "Can't find a suitable pixelformat", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      if (!SetPixelFormat(hDC, PixelFormat, &pfd))
      {
            MessageBox(NULL, "Can't set the pixel format", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      if (!(hRC = wglCreateContext(hDC)))
      {
            MessageBox(NULL, "Can't create a GL rendering context", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      if (!wglMakeCurrent(hDC, hRC))
      {
            MessageBox(NULL, "Can't activate the GL rendering context", "Error", MB_OK | MB_ICONEXCLAMATION);

            return FALSE;
      }

      void * nullTester = NULL;
      HINSTANCE hGLLIB = NULL;
      hGLLIB = LoadLibrary("opengl32.dll");

      if (hGLLIB == NULL)
      {
            return FALSE;
      }

      nullTester = glActiveTexture                    = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
      nullTester = glAttachShader                     = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
      nullTester = glBindBuffer                       = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
      nullTester = glBufferData                       = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
      nullTester = glCompileShader                    = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
      nullTester = glCreateProgram                    = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
      nullTester = glCreateShader                     = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
      nullTester = glDeleteShader                     = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
      nullTester = glDetachShader                     = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
      nullTester = glDisableVertexAttribArray         = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
      nullTester = glEnableVertexAttribArray          = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
      nullTester = glGenBuffers                       = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
      nullTester = glGetShaderInfoLog                 = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
      nullTester = glGetShaderiv                      = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
      nullTester = glGetUniformfv                     = (PFNGLGETUNIFORMFVPROC)wglGetProcAddress("glGetUniformfv");
      nullTester = glGetUniformLocation               = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
      nullTester = glLinkProgram                      = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
      nullTester = glShaderSource                     = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
      nullTester = glUniform1i                        = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
      nullTester = glUniform2f                        = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
      nullTester = glUseProgram                       = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
      nullTester = glVertexAttribPointer              = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
      
      if (nullTester == NULL)
      {
            return FALSE;
      }

      nullTester = glBindTexture                      = (PFNGLBINDTEXTUREPROC)GetProcAddress(hGLLIB, "glBindTexture");
      nullTester = glClear                            = (PFNGLCLEARPROC)GetProcAddress(hGLLIB, "glClear");
      nullTester = glClearColor                       = (PFNGLCLEARCOLORPROC)GetProcAddress(hGLLIB, "glClearColor");
      nullTester = glClearDepth                       = (PFNGLCLEARDEPTHPROC)GetProcAddress(hGLLIB, "glClearDepth");
      nullTester = glDepthFunc                        = (PFNGLDEPTHFUNCPROC)GetProcAddress(hGLLIB, "glDepthFunc");
      nullTester = glDrawArrays                       = (PFNGLDRAWARRAYSPROC)GetProcAddress(hGLLIB, "glDrawArrays");
      nullTester = glEnable                           = (PFNGLENABLEPROC)GetProcAddress(hGLLIB, "glEnable");
      nullTester = glGenTextures                      = (PFNGLGENTEXTURESPROC)GetProcAddress(hGLLIB, "glGenTextures");
      nullTester = glTexImage2D                       = (PFNGLTEXIMAGE2DPROC)GetProcAddress(hGLLIB, "glTexImage2D");
      nullTester = glTexParameteri                    = (PFNGLTEXPARAMETERIPROC)GetProcAddress(hGLLIB, "glTexParameteri");
      nullTester = glViewport                         = (PFNGLVIEWPORTPROC)GetProcAddress(hGLLIB, "glViewport");
      nullTester = glDrawElements                     = (PFNGLDRAWELEMENTSPROC)GetProcAddress(hGLLIB, "glDrawElements");
      nullTester = glGetError                         = (PFNGLGETERRORPROC)GetProcAddress(hGLLIB, "glGetError");
      nullTester = glGetString                        = (PFNGLGETSTRINGPROC)GetProcAddress(hGLLIB, "glGetString");

      if (nullTester == NULL)
      {
            return FALSE;
      }

      ShowWindow(hWnd, globCmdShow);
      SetForegroundWindow(hWnd);
      UpdateWindow(hWnd);

      return TRUE;
}
