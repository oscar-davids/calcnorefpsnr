import ctypes

import os.path
from ctypes import *

lipath = os.path.abspath(os.path.dirname(__file__))
lib = cdll.LoadLibrary(os.path.join(lipath, ".", "calcnorefpsnr.dll"))


#lib = ctypes.cdll.LoadLibrary("./libclibhash.so.5.1.0")
#lib = ctypes.cdll.LoadLibrary("D:/work/vqwork/calcnorefpsnr/calcnorefpsnr.dll")
lib.calc_norefpsnr.restype = ctypes.c_float
#lib.calc_norefpsnr.argtypes = [POINTER(c_char)]


class calcnorefpsnr:
    binit = 0    
    # Initializing  
    def __init__(self):
        #print('calcnorefpsnr Initializing')
        if calcnorefpsnr.binit == 0:            
            calcnorefpsnr.binit = 1  
    # Destroying 
    def __del__(self):
        #print("calcnorefpsnr Destroying")
        if calcnorefpsnr.binit == 1:            
            calcnorefpsnr.binit = 0

    def __del__(self):
        #print("calcnorefpsnr Destroying")
        if calcnorefpsnr.binit == 1:            
            calcnorefpsnr.binit = 0
            
    def calc_norefpsnr(self, videopath):

        strpath = videopath.encode('utf-8')

        fpsnr = lib.calc_norefpsnr(strpath)
        return fpsnr
