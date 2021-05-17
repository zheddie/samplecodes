#!/qopensys/pkgs/bin/python3
import os as _os
import io as _io
print("_os.O_RDWR:%#x,_os.O_CREAT:%#x,_os.O_EXCL:%#x"%(_os.O_RDWR,_os.O_CREAT,_os.O_EXCL))

_text_openflags = _os.O_RDWR | _os.O_CREAT | _os.O_EXCL
if hasattr(_os, 'O_NOFOLLOW'):
    _text_openflags |= _os.O_NOFOLLOW
    print("_os.O_NOFOLLOW:%#x"%_os.O_NOFOLLOW)
	


_bin_openflags = _text_openflags
if hasattr(_os, 'O_BINARY'):
    _bin_openflags |= _os.O_BINARY
    print("_os.O_BINARY:%#x"%_os.O_BINARY)

filename = "/tmp/zgtmp01"
print("zg.tempfile.py.filename:",filename,_bin_openflags)
fd = _os.open(filename, _bin_openflags, 0o600)

try:
    with _io.open(fd, 'wb', closefd=False) as fp:
        fp.write(b'blat')
finally:
    _os.close(fd)