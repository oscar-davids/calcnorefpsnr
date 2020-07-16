
#define DllImport   __declspec( dllimport )
#define DllExport   __declspec( dllexport )

#ifdef _WINDLL
DllExport float calc_norefpsnr(char* vpath);
#else
DllImport float calc_norefpsnr(char* vpath);
#endif