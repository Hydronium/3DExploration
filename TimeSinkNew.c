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
ATOM resultRegisterClass;

HANDLE eventResTimer;
HANDLE hResTimer = NULL;
HANDLE hTimerQueue = NULL;

HDC         hDC         = NULL;
HGLRC       hRC         = NULL;

bool FULLSCREEN = FALSE;
bool INFOCUS = TRUE;

int globCmdShow;
int width, height;

short int unconsumedEvents = 0;

GLenum enumGLError;

GLuint myProgram;

GLuint vPosBufferObject;

const float vPos[] =
{
      -0.1f,  1.0f,
      -1.0f, -0.5f,
       0.0f,  1.0f,
};
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

int GetBitmap(void); //At the moment this works as intended. Future: overlay a struct on the memory.
int Get4Bytes(FILE * ptr);
short int Get2Bytes(FILE * ptr);

bool Render(void);

int CreateProgram(void);

void InitGLSettings(void);

void ResizeViewport(int width, int height);
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
            return 1;
      }

      //Do we want fullscreen? Not needed currently but keeping it so I don't forget.
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
      glViewport(0, 0, width, height);
      InitGLSettings();


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

            Render();
            SwapBuffers(hDC);
      }

      return (Msg.wParam);
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
            case WM_SIZE:
            {

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

      if (hGLLIB == NULL)
      {
            GetLastError();
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

      glGetIntegerv                      = (PFNGLGETINTEGERVPROC)GetProcAddress(hGLLIB, "glGetIntegerv");

      glBindTexture                      = (PFNGLBINDTEXTUREPROC)GetProcAddress(hGLLIB, "glBindTexture");
      glClear                            = (PFNGLCLEARPROC)GetProcAddress(hGLLIB, "glClear");
      glClearColor                       = (PFNGLCLEARCOLORPROC)GetProcAddress(hGLLIB, "glClearColor");
      glClearDepth                       = (PFNGLCLEARDEPTHPROC)GetProcAddress(hGLLIB, "glClearDepth");
      glDepthFunc                        = (PFNGLDEPTHFUNCPROC)GetProcAddress(hGLLIB, "glDepthFunc");
      glDrawArrays                       = (PFNGLDRAWARRAYSPROC)GetProcAddress(hGLLIB, "glDrawArrays");
      glDrawElements                     = (PFNGLDRAWELEMENTSPROC)GetProcAddress(hGLLIB, "glDrawElements");
      glEnable                           = (PFNGLENABLEPROC)GetProcAddress(hGLLIB, "glEnable");
      glGenTextures                      = (PFNGLGENTEXTURESPROC)GetProcAddress(hGLLIB, "glGenTextures");
      glGetError                         = (PFNGLGETERRORPROC)GetProcAddress(hGLLIB, "glGetError");
      glGetString                        = (PFNGLGETSTRINGPROC)GetProcAddress(hGLLIB, "glGetString");
      glTexImage2D                       = (PFNGLTEXIMAGE2DPROC)GetProcAddress(hGLLIB, "glTexImage2D");
      glTexParameteri                    = (PFNGLTEXPARAMETERIPROC)GetProcAddress(hGLLIB, "glTexParameteri");
      glViewport                         = (PFNGLVIEWPORTPROC)GetProcAddress(hGLLIB, "glViewport");

      if(GetLastError())
      {
            return FALSE;
      }

      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClearDepth(1.0f);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LEQUAL);

      if (GetBitmap() != 0)
      {
            GetLastError();
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
            if (unconsumedEvents < 15) //If we've missed 15 let's stop firing until it drops. Wouldn't want to overflow.
            {      
                  SetEvent(eventResTimer); 
                  unconsumedEvents++;
            }
      }
}

short int Get2Bytes(FILE * ptr)
{
      short int val = 0;

      val = (int)(fgetc(ptr));
      val |= ((int)(fgetc(ptr)) << 8);

      return val;
}

int Get4Bytes(FILE * ptr)
{
      int val = 0;

      val = (int)(fgetc(ptr));
      val |= ((int)(fgetc(ptr)) << 8);
      val |= ((int)(fgetc(ptr)) << 16);
      val |= ((int)(fgetc(ptr)) << 24);

      return val;

}

int GetBitmap(void)
{
      char        * bmBuffer, * pxPtr;
      FILE        * bmFile;
      GLuint      texture;

      int         bmSize, 
                  dataOffset, 
                  dibSize, 
                  bmWidthPx, 
                  bmHeightPx,
                  bmCompression,
                  dataSize,
                  dataHorRes,
                  dataVerRes,
                  paletteNumClrs,
                  importantClrs,
                  
                  bmBytesPerPixel = 3, //Default -> RGB

                  totalBytesPerRow,
                  pixelBytesPerRow,
                  padCount = 0;

      short int   bmClrPlane, bmBPP;
      char        bmChar0, bmChar1;


      bmFile = fopen("multisquare.bmp", "rb");
      if (bmFile == NULL)
      {
            return 1;
      }

      bmChar0 = fgetc(bmFile);
      bmChar1 = fgetc(bmFile);
      if (bmChar0 != 'B' || bmChar1 != 'M')
      {
            return 2;
      }

      //sprintf(msgData, "%c%c", bmChar0, bmChar1);
      //MessageBox(NULL, msgData, NULL, MB_OK | MB_ICONINFORMATION);
      
      bmSize = Get4Bytes(bmFile);

      //Skip 4 bytes. These bytes are application specific,
      //and generally unused.
      if (fseek(bmFile, 4, SEEK_CUR) != 0)
      {
            return 3;
      }

      dataOffset = Get4Bytes(bmFile);
      
      dibSize = Get4Bytes(bmFile);

      //Replace 'if dibSize' check with case statement
      //which branches to functions for different sized 
      //DIBHeaders.
      //
      //
      if (dibSize != 40)
      {
            return 4;
      }

      bmWidthPx = Get4Bytes(bmFile);
      bmHeightPx = Get4Bytes(bmFile); //Later -> handle negative = top->bottom.

      bmClrPlane = Get2Bytes(bmFile); //Must always be 1 anyways, consider removing this and skipping 2 bytes.
      bmBPP = Get2Bytes(bmFile);

      if (bmBPP == 24)
      {
            bmBytesPerPixel = 3;
      }

      bmCompression = Get4Bytes(bmFile);
      //Handle other compressions at some later time.
      if (bmCompression != 0)
      {
            return 5;
      }

      //Can use this to allocate appropriate memory space.
      dataSize = Get4Bytes(bmFile);

      //Resolutions doesn't seem too important atm.
      dataHorRes = Get4Bytes(bmFile);
      dataVerRes = Get4Bytes(bmFile);

      //Will probably both be 0. Irrelevant atm.
      paletteNumClrs = Get4Bytes(bmFile);
      importantClrs = Get4Bytes(bmFile);

      bmBuffer = (char *) calloc(dataSize, sizeof(char)); //Space allocated.

      fseek(bmFile, dataOffset, SEEK_SET);

      //Ex: 10 pixels * 3 bytes/pixel = 30 bytes
      //    30 + 3 = 33 -> 0010 0001
      //    Right shift 2: 0000 1000  -> These operations round to nearest
      //    Shift left 2:  0010 0000  -> multiple of 4.
      //    32 bytes to reach 4byte multiple
      //    So 30 bytes for 10 pixles plus 2 extra bytes of padding, per row.
      pixelBytesPerRow = bmWidthPx * bmBytesPerPixel;
      totalBytesPerRow = ((pixelBytesPerRow + bmBytesPerPixel) >> 2) << 2;
      padCount = totalBytesPerRow - pixelBytesPerRow;

      pxPtr = bmBuffer;

      switch(padCount)
      {
            case 0:
            {
                  for (int A = 0; A <= bmHeightPx; A++)
                  {
                        fread(pxPtr, 1, pixelBytesPerRow, bmFile);
                        pxPtr += totalBytesPerRow;
                  }

                  break;
            }
            case 1:
            case 2:
            case 3:
            {
                  for (int A = 0; A <= bmHeightPx; A++)
                  {
                        fread(pxPtr, 1, pixelBytesPerRow, bmFile);

                        if (fseek(bmFile, padCount, SEEK_CUR) != 0)
                        {
                              return 3;
                        }

                        pxPtr += totalBytesPerRow;

                  }

                  break;
            }
            default:
            //Shouldn't get here
            break;
      }

      fclose(bmFile);

      glActiveTexture(GL_TEXTURE0);

      glGenTextures(1, &texture);

      glBindTexture(GL_TEXTURE_2D, texture);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      glTexImage2D(GL_TEXTURE_2D,   /*Type of texture*/
                   0,               /*Level of Detail number*/
                   GL_RGB,          /*Internal format*/
                   bmWidthPx,       /*Width in texels(pixels?)*/
                   bmHeightPx,      /*Height in texels(pixels?)*/
                   0,               /*Border. Must be 0 (probably only for 2D)*/
                   GL_BGR,          /*Format, of the data the texture will be created from*/
                   GL_UNSIGNED_BYTE,/*Data type of the pixel data*/
                   bmBuffer);       /*Pointer to the image data to create the texture from*/

      //glBindTexture(GL_TEXTURE_2D, 0);

      free(bmBuffer);


      return 0;
}

bool Render(void)
{
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glUseProgram(myProgram);
      glBindBuffer(GL_ARRAY_BUFFER, vPosBufferObject);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
     
      glEnableVertexAttribArray(0);
      glDrawArrays(GL_TRIANGLES, 0, 6);

      glDisableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glUseProgram(0);
      return TRUE;
}

int CreateProgram(void)
{
      unsigned int rcount1; //Store fread result for manual checking (not currently coded)
      unsigned int rcount2; //Store fread result for manual checking (not currently coded)
      char * dataBuffer;
      struct _stat fileStat;
      long fLen;
      FILE * vSh;
      FILE * fSh;
      int desc;

      GLint status;
      GLint logLength;

      GLuint vShader = glCreateShader(GL_VERTEX_SHADER);

      GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

      GLuint program = glCreateProgram();

      /*__________________________________________________*/

      vSh = fopen("vShader.vSh", "r");
      desc = _fileno(vSh);
      _fstat(desc, &fileStat);

      fLen = fileStat.st_size;

      dataBuffer = (char *) calloc((fLen + 1), sizeof(char));
      if (dataBuffer == NULL)
      {
            MessageBox(NULL, "Malloc failure: VS", "Create Program", MB_OK | MB_ICONINFORMATION);
            return 1;
      }

      rcount1 = fread(dataBuffer, 1, fLen, vSh);   
      
      if(feof(vSh) == 0)
      {
            MessageBox(NULL, "Did not reach EoF for VS", "Create Program", MB_OK | MB_ICONINFORMATION);
            return 2;
      }
      fclose(vSh);
      
      glShaderSource(vShader, 1, &dataBuffer, NULL);
      glCompileShader(vShader);

      //Error checking
      glGetShaderiv(vShader, GL_COMPILE_STATUS, &status);
      if (status == GL_FALSE)
      {
            glGetShaderiv(vShader, GL_INFO_LOG_LENGTH, &logLength);
            char resultV[logLength+1];
            glGetShaderInfoLog(vShader, logLength, NULL, resultV);

            MessageBox(NULL, resultV, "Vertex Compile Error", MB_OK);
      }
      free(dataBuffer);

      fSh = fopen("fShader.fSh", "r");
      desc = _fileno(fSh);
      _fstat(desc, &fileStat);

      fLen = fileStat.st_size;
      dataBuffer = (char *) calloc((fLen + 1), sizeof(char));

      if (dataBuffer == NULL)
      {
            MessageBox(NULL, "Malloc failure: FS", "Create Program", MB_OK | MB_ICONINFORMATION);
           return 3;
      }

      rcount2 = fread(dataBuffer, 1, fLen, fSh);

      if(feof(fSh) == 0)
      {
            MessageBox(NULL, "Did not reach EoF for FS", "Create Program", MB_OK | MB_ICONINFORMATION);
            return 4;
      }
      fclose(fSh);

      glShaderSource(fShader, 1, &dataBuffer, NULL);
      glCompileShader(fShader);

      //Error checking
      glGetShaderiv(fShader, GL_COMPILE_STATUS, &status);
      if (status == GL_FALSE)
      {
            glGetShaderiv(fShader, GL_INFO_LOG_LENGTH, &logLength);
            char resultF[logLength+1];
            glGetShaderInfoLog(fShader, logLength, NULL, resultF);

            MessageBox(NULL, resultF, "Vertex Compile Error", MB_OK);
      }

      free(dataBuffer); //Memory freed. dataBuffer itself will be destroyed on return automatically.
      /*__________________________________________________*/


      glAttachShader(program, vShader);
      glAttachShader(program, fShader);
      
      glLinkProgram(program);
      
      glDetachShader(program, vShader);
      glDetachShader(program, fShader);

      glDeleteShader(vShader);
      glDeleteShader(fShader);

      return program;
}

void InitGLSettings(void)
{
      //char * glVer = (char *)glGetString(GL_VERSION);
      //char * glSLV = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClearDepth(1.0f);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LEQUAL);

      glGenBuffers(1, &vPosBufferObject);

      glBindBuffer(GL_ARRAY_BUFFER, vPosBufferObject);

      glBufferData(GL_ARRAY_BUFFER, sizeof(vPos), vPos, GL_STATIC_DRAW);

      glBindBuffer(GL_ARRAY_BUFFER, 0);

      myProgram = CreateProgram();
}

void ResizeViewport(int width, int height)
{
      if (height == 0)
      {
            height = 1;
      }
      
      glViewport(0, 0, width, height);
}
