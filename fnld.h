
/* Oct 23, 2012*/
/* 20170327 - Since GL_GLEXT_PROTOTYPES is not defined, and we do not include anything that defines it, glcorearb.h does not expose the function prototypes for the names below.
	This allows me to use the typical OpenGL function names for my function pointers, using the typedefs exposed by glcorearb.h (which are not locked behind GL_GLEXT_PROTOTYPES). */
PFNGLACTIVETEXTUREPROC				glActiveTexture;
PFNGLATTACHSHADERPROC				glAttachShader;
PFNGLBINDBUFFERPROC					glBindBuffer;
PFNGLBINDTEXTUREPROC				glBindTexture;
PFNGLBUFFERDATAPROC					glBufferData;
PFNGLCLEARCOLORPROC					glClearColor;
PFNGLCLEARDEPTHPROC					glClearDepth;
PFNGLCLEARPROC						glClear;
PFNGLCOMPILESHADERPROC				glCompileShader;
PFNGLCREATEPROGRAMPROC 				glCreateProgram;
PFNGLCREATESHADERPROC 				glCreateShader;
PFNGLDELETESHADERPROC				glDeleteShader;
PFNGLDEPTHFUNCPROC					glDepthFunc;
PFNGLDETACHSHADERPROC				glDetachShader;
PFNGLDISABLEVERTEXATTRIBARRAYPROC	glDisableVertexAttribArray;
PFNGLDRAWARRAYSPROC					glDrawArrays;
PFNGLDRAWELEMENTSPROC				glDrawElements;
PFNGLENABLEPROC						glEnable;
PFNGLENABLEVERTEXATTRIBARRAYPROC 	glEnableVertexAttribArray;
PFNGLGENBUFFERSPROC					glGenBuffers;
PFNGLGENTEXTURESPROC				glGenTextures;
PFNGLGETERRORPROC					glGetError;
PFNGLGETSHADERINFOLOGPROC			glGetShaderInfoLog;
PFNGLGETSHADERIVPROC				glGetShaderiv;
PFNGLGETSTRINGPROC					glGetString;
PFNGLGETUNIFORMFVPROC				glGetUniformfv;
PFNGLGETUNIFORMLOCATIONPROC			glGetUniformLocation;
PFNGLLINKPROGRAMPROC				glLinkProgram;
PFNGLSHADERSOURCEPROC				glShaderSource;
PFNGLTEXIMAGE2DPROC					glTexImage2D;
PFNGLTEXPARAMETERIPROC				glTexParameteri;
PFNGLUNIFORM1IPROC					glUniform1i;
PFNGLUNIFORM2FPROC					glUniform2f;
PFNGLUSEPROGRAMPROC					glUseProgram;
PFNGLVERTEXATTRIBPOINTERPROC		glVertexAttribPointer;
PFNGLVIEWPORTPROC					glViewport;
PFNGLGETINTEGERVPROC				glGetIntegerv;
PFNGLUNIFORMMATRIX4FVPROC			glUniformMatrix4fv;

/* 20170327 - Unfortunately I cannot determine a way to avoid wingdi.h or otherwise use the original names for these functions. Prefixed with "MY" to stay close to the name. */
typedef HGLRC WINAPI (*PFNwglCreateContext)(HDC);
typedef BOOL WINAPI (*PFNwglMakeCurrent)(HDC,HGLRC);
typedef PROC WINAPI (*PFNwglGetProcAddress)(LPCSTR); /*Pelles C warns "Missing prototype for 'PFNwglGetProcAddress', with type 'long long int __stdcall (*__stdcall (*)(const char *))()'", which I believe is due to the empty braces at the end of the type. Does not seem to harm anything, but it is irritating to be warned of something when there is no apparent way around it. The definition of PROC and WINAPI seem to be cause, however innocuous the effect. */

PFNwglCreateContext MYwglCreateContext;
PFNwglMakeCurrent MYwglMakeCurrent;
PFNwglGetProcAddress MYwglGetProcAddress;
