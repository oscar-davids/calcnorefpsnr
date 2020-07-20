#ifndef _NOREFPSNR_H
#define _NOREFPSNR_H

#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

#ifdef _WIN32
#ifdef _WINDLL
DllExport float calc_norefpsnr(char* vpath);
#else
DllImport float calc_norefpsnr(char* vpath);
#endif
#else //_WIN32

#ifdef __cplusplus
extern "C" {
#endif

float calc_norefpsnr(char* vpath);

#ifdef __cplusplus
}
#endif

#endif //_WIN32
#endif //_NOREFPSNR_H
