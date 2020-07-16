import ctypes
import platform
import os.path

lipath = os.path.abspath(os.path.dirname(__file__))
if platform.system() == "Windows":
    lib = ctypes.cdll.LoadLibrary(os.path.join(lipath, ".", "calcnorefpsnr.dll"))
else:
    lib = ctypes.cdll.LoadLibrary(os.path.join(lipath, ".", "calcnorefpsnr.so"))

lib.calc_norefpsnr.restype = ctypes.c_float

class norefpsnr:
    def calc_norefpsnr(self, videopath):
        strpath = videopath.encode('utf-8')
        fpsnr = lib.calc_norefpsnr(strpath)
        return fpsnr

mycalc = norefpsnr()
mycalc.calc_norefpsnr("1.h264")

