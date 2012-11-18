
#include <windows.h>
#include <sys/stat.h>
//#include <gl/gl.h>
#include "glcorearb.h"
#include "fnld.h"
//#include <gl/glu.h>
 
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

float vertexPositions[6600];

float screen_width = 0.0, screen_height = 0.0;

/*
const float vertexPositions[] = {
      -1.0f, -0.8f, //0
      -1.0f, -1.0f, //1

      -0.8f, -0.8f, //2
      -0.8f, -1.0f, //3

      -0.8f, -0.8f, //2
      -0.8f, -1.0f, //3

      -0.6f, -0.8f, //4
      -0.6f, -1.0f, //5

      -0.6f, -0.8f, //4
      -0.6f, -1.0f, //5

      -0.4f, -0.8f, //6
      -0.4f, -1.0f, //7

      -0.4f, -0.8f, //6
      -0.4f, -1.0f, //7

      -0.2f, -0.8f, //8
      -0.2f, -1.0f, //9

      -0.2f, -0.8f, //8
      -0.2f, -1.0f, //9

       0.0f, -0.8f, //10
       0.0f, -1.0f, //11

       0.0f, -0.8f, //10
       0.0f, -1.0f, //11

///////////////////////////////////
       0.0f,  1.0f, //0
       0.0f,  0.0f, //1

       1.0f,  1.0f, //2
       1.0f,  0.0f, //3

       0.0f,  1.0f, //2
       0.0f,  0.0f, //3

       1.0f,  1.0f, //4
       1.0f,  0.0f, //5

       0.0f,  1.0f, //4
       0.0f,  0.0f, //5

       1.0f,  1.0f, //6
       1.0f,  0.0f, //7

       0.0f,  1.0f, //6
       0.0f,  0.0f, //7

       1.0f,  1.0f, //8
       1.0f,  0.0f, //9

       0.0f,  1.0f, //8
       0.0f,  0.0f, //9

       1.0f,  1.0f, //10
       1.0f,  0.0f, //11

       0.0f,  1.0f, //10
       0.0f,  0.0f, //11
****
      -1.0f, -0.8f, //0
      -0.8f, -0.8f, //1
      -0.6f, -0.8f, //2
      -0.4f, -0.8f, //3
      -0.2f, -0.8f, //4
       0.0f, -0.8f, //5

      -1.0f, -1.0f, //6
      -0.8f, -1.0f, //7
      -0.6f, -1.0f, //8
      -0.4f, -1.0f, //9
      -0.2f, -1.0f, //10
       0.0f, -1.0f, //11

      0.00f, 1.00f,
      0.00f, 0.00f,
      0.00f, 1.00f,
      1.00f, 1.00f,
      0.00f, 1.00f,
      1.00f, 1.00f,

      0.00f, 0.00f,
      1.00f, 0.00f,
      0.00f, 0.00f,
      1.00f, 0.00f,
      0.00f, 0.00f,
      1.00f, 0.00f,

};
*/
const GLubyte indices[] = {
      0, 1, 2, 
      3, 2, 1,

      4, 5, 6,
      7, 6, 5, 

      8, 9, 10,
      11, 10, 9,

      12, 13, 14,
      15, 14, 13,

      16, 17, 18,
      19, 18, 17,
};

/*--------------------------------------------------*/ 
LRESULT CALLBACK WndProc(     HWND hWnd,
                              UINT uMSG,
                              WPARAM wParam,
                              LPARAM lParam);
 
GLuint CreateProgram(GLvoid);
 
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

int GetBitmap(void);

int Get4Bytes(FILE * ptr);
short int Get2Bytes(FILE * ptr);

int blah(void);
/*--------------------------------------------------*/

int blah(void)
{
      int count = 0, texture_start;

      for (int y = 0; y < 20; y++)
      {
            for (int x = 0; x < 21; x++)
            {
                  vertexPositions[count++] = x * 32.0f;
                  vertexPositions[count++] = (y + 1) * 32.0f;

                  vertexPositions[count++] = x * 32.0f;                  
                  vertexPositions[count++] = y * 32.0f;

                  if (x > 0 && x < 20)
                  {
                        vertexPositions[count++] = x * 32.0f;
                        vertexPositions[count++] = (y + 1) * 32.0f;

                        vertexPositions[count++] = x * 32.0f;                  
                        vertexPositions[count++] = y * 32.0f;
                  }
            }
      }
      
      texture_start = count;

      for (int z = 0; z < 400; z++)
      {
                  vertexPositions[count++] = 0.0f;
                  vertexPositions[count++] = 1.0f;
      
                  vertexPositions[count++] = 0.0f;
                  vertexPositions[count++] = 0.0f;

                  vertexPositions[count++] = 1.0f;
                  vertexPositions[count++] = 1.0f;

                  vertexPositions[count++] = 1.0f;
                  vertexPositions[count++] = 0.0f;
      }

      return 0;
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

GLuint CreateProgram(GLvoid)
{
      unsigned int rcount1;
      unsigned int rcount2;
      char * dataBuffer;
      struct _stat fileStat;
      long fLen;
      FILE * vSh;
      FILE * fSh;
      int desc;

      GLint status;
      GLint logLength;

      GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
      //errort = glGetError();
      GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
      //errort = glGetError();
      GLuint program = glCreateProgram();
      //errort = glGetError();
      /*__________________________________________________*/

      vSh = fopen("vShader.vSh", "r");
      desc = _fileno(vSh);
      _fstat(desc, &fileStat);

      fLen = fileStat.st_size;
      //dataBuffer = (char *) malloc((fLen * sizeof(char)) + sizeof(char));
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
 
GLvoid InitGL(GLvoid)
{
      //blah();
      FnLdInit();
      GetBitmap();

      //char * glVer = (char *)glGetString(GL_VERSION);
      //char * glSLV = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

      glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
      glClearDepth(1.0f);
      glEnable(GL_DEPTH_TEST);
      glDepthFunc(GL_LEQUAL);

      glGenBuffers(1, &positionBufferObject);
      //errort = glGetError();

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, positionBufferObject);
      //errort = glGetError();
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexPositions), (const float *)vertexPositions, GL_STATIC_DRAW);
      //errort = glGetError();
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      //errort = glGetError();
 
      myProgram = CreateProgram();
 
}
 
int DrawGLScene(GLvoid)
{
      //float uniforms[2];
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
      glUniform1i(/*textureLocation*/glGetUniformLocation(myProgram, "textureS2D"), 0);
      glUniform2f(glGetUniformLocation(myProgram, "screenDim"), screen_width, screen_height);
      //glGetUniformfv(myProgram, glGetUniformLocation(myProgram, "screenDim"), uniforms);
      //sprintf(msgData, "%f, %f", uniforms[0], uniforms[1]);
      //MessageBox(NULL, msgData, NULL, MB_OK | MB_ICONINFORMATION);
      //glBindTexture(GL_TEXTURE_2D, texture);

      //Before
      //glDrawArrays(GL_TRIANGLES, 0, 6);
      glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_BYTE, indices);

      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
      glUseProgram(0);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 
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
          16,                             // 16Bit Z-Buffer (Depth Buffer)
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

      //char * glVer = (char *)glGetString(GL_VERSION);
      //char * glSLV = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);

      return TRUE;
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

      if (!CreateGLWindow("My window", (int)screen_width, (int)screen_height, 24, fullscreen))
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
