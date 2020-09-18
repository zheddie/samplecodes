import threading
def countdown(n):
	while n > 0:
		n -= 1
COUNT=10000000
#countdown(COUNT)
t1 = threading.Thread(target=countdown,args=(COUNT//2,))
t2 = threading.Thread(target=countdown,args=(COUNT//2,))
t1.start()
t2.start()
t1.join()
t2.join()
