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

bool FULLSCREEN = FALSE;
bool INFOCUS = TRUE;

HANDLE eventResTimer;
HANDLE hResTimer = NULL;
HANDLE hTimerQueue = NULL;
short int unconsumedEvents = 0;
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

VOID CALLBACK cbResTimer(     PVOID lpParam,          //Optional data from CreateTimerQueueTimer
                              BOOLEAN hasFired);      //True for timers, false for wait events

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

      DWORD resultWaitResTimer = 0;

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
      if (FULLSCREEN)
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
                        FULLSCREEN = FALSE;
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

            resultWaitResTimer = WaitForSingleObject(eventResTimer, 17);
            if (resultWaitResTimer == WAIT_FAILED)
            {
                  GetLastError();
                  return -1;
            }
            else if (resultWaitResTimer == WAIT_OBJECT_0)
            {
                  unconsumedEvents--;
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
            case WM_ACTIVATE:
            {
                  if (LOWORD(wParam))
                  {
                        INFOCUS = TRUE;
                  }
                  else
                  {
                        INFOCUS = FALSE;
                  }
                  break;
            }
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
                  CloseHandle(eventResTimer);
                  if (!DeleteTimerQueue(hTimerQueue))
                  {
                        GetLastError();
                  }
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

      HINSTANCE hGLLIB = NULL;
      hGLLIB = LoadLibrary("opengl32.dll");

      if(GetLastError())
      {
            return FALSE;
      }

      glActiveTexture                    = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
      glAttachShader                     = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
      glBindBuffer                       = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
      glBufferData                       = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
      glCompileShader                    = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
      glCreateProgram                    = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
      glCreateShader                     = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
      glDeleteShader                     = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
      glDetachShader                     = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
      glDisableVertexAttribArray         = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
      glEnableVertexAttribArray          = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
      glGenBuffers                       = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
      glGetShaderInfoLog                 = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
      glGetShaderiv                      = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
      glGetUniformfv                     = (PFNGLGETUNIFORMFVPROC)wglGetProcAddress("glGetUniformfv");
      glGetUniformLocation               = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
      glLinkProgram                      = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
      glShaderSource                     = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
      glUniform1i                        = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
      glUniform2f                        = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
      glUseProgram                       = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
      glVertexAttribPointer              = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
      
      glBindTexture                      = (PFNGLBINDTEXTUREPROC)GetProcAddress(hGLLIB, "glBindTexture");
      glClear                            = (PFNGLCLEARPROC)GetProcAddress(hGLLIB, "glClear");
      glClearColor                       = (PFNGLCLEARCOLORPROC)GetProcAddress(hGLLIB, "glClearColor");
      glClearDepth                       = (PFNGLCLEARDEPTHPROC)GetProcAddress(hGLLIB, "glClearDepth");
      glDepthFunc                        = (PFNGLDEPTHFUNCPROC)GetProcAddress(hGLLIB, "glDepthFunc");
      glDrawArrays                       = (PFNGLDRAWARRAYSPROC)GetProcAddress(hGLLIB, "glDrawArrays");
      glEnable                           = (PFNGLENABLEPROC)GetProcAddress(hGLLIB, "glEnable");
      glGenTextures                      = (PFNGLGENTEXTURESPROC)GetProcAddress(hGLLIB, "glGenTextures");
      glTexImage2D                       = (PFNGLTEXIMAGE2DPROC)GetProcAddress(hGLLIB, "glTexImage2D");
      glTexParameteri                    = (PFNGLTEXPARAMETERIPROC)GetProcAddress(hGLLIB, "glTexParameteri");
      glViewport                         = (PFNGLVIEWPORTPROC)GetProcAddress(hGLLIB, "glViewport");
      glDrawElements                     = (PFNGLDRAWELEMENTSPROC)GetProcAddress(hGLLIB, "glDrawElements");
      glGetError                         = (PFNGLGETERRORPROC)GetProcAddress(hGLLIB, "glGetError");
      glGetString                        = (PFNGLGETSTRINGPROC)GetProcAddress(hGLLIB, "glGetString");

      if(GetLastError())
      {
            return FALSE;
      }

      eventResTimer = CreateEvent(  NULL,       //No SECURITY_ATTRIBUTES struct
                                    FALSE,      //Auto-reset
                                    FALSE,      //Initially off
                                    NULL);      //No name

      if(GetLastError())
      {
            return FALSE;
      }

      hTimerQueue = CreateTimerQueue();

      if(GetLastError())
      {
            return FALSE;
      }

      if (!CreateTimerQueueTimer(   &hResTimer,                         //Address of timer handle
                                    hTimerQueue,                        //Which queue to put it in
                                    (WAITORTIMERCALLBACK)cbResTimer,    //What callback function to use
                                    NULL,                               //No optional parameter
                                    20,                                 //20Ms till first fire
                                    8,                                  //8Ms period between fires
                                    WT_EXECUTEINTIMERTHREAD))           //Execute in this thread
      {
        GetLastError();
        return FALSE;
      }

      ShowWindow(hWnd, globCmdShow);
      SetForegroundWindow(hWnd);
      UpdateWindow(hWnd);

      return TRUE;
}

VOID CALLBACK cbResTimer(     PVOID lpParam,
                              BOOLEAN hasFired)
{
      if (hasFired)
      {
            SetEvent(eventResTimer);
            unconsumedEvents++;
      }
}
