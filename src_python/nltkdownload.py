import socks
import socket
socks.setdefaultproxy(socks.PROXY_TYPE_SOCKS5,"127.0.0.1",9998)
socket.socket = socks.socksocket
import nltk
nltk.download()
