import threading
def countdown(n):
	while n > 0:
		n -= 1
COUNT=10000000
#countdown(COUNT)
t1 = threading.Thread(target=countdown,args=(COUNT//4,))
t2 = threading.Thread(target=countdown,args=(COUNT//4,))
t3 = threading.Thread(target=countdown,args=(COUNT//4,))
t4 = threading.Thread(target=countdown,args=(COUNT//4,))
t1.start()
t2.start()
t3.start()
t4.start()
t1.join()
t2.join()
t3.join()
t4.join()
