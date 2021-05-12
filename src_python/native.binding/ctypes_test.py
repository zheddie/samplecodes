# ctypes_test.py
import ctypes
import pathlib

if __name__ == "__main__":
    # Load the shared library into ctypes
    libname = pathlib.Path().absolute() / "libcmult.so"
    c_lib = ctypes.CDLL(libname)
    x, y = 6, 2.3
    c_lib.cmult.restype = ctypes.c_float
    answer = c_lib.cmult(x, ctypes.c_float(y))
    print(f"{x}*{y:.1f}={answer:.1f}")
