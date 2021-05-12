import asyncio
import time
from threading import Thread

async def func1():
    print("I am in func1.1")
    # time.sleep(3)
    await asyncio.sleep(3)
    print("I am in func1.2")
async def func2():
    print("I am in func2.1")
    # time.sleep(2)
    await asyncio.sleep(2)
    print("I am in func2.2")
async def func3():
    print("I am in func3.1")
    # time.sleep(4)
    await asyncio.sleep(4)
    print("I am in func3.2")

if __name__ == "__main__":
    start = time.time()
    f1 = func1()
    f2 = func2()
    f3 = func3()
    flist=[f1,f2,f3]
    asyncio.run(asyncio.wait(flist))
    
    # t1 = Thread(target=func1)
    # t2 = Thread(target=func2)
    # t3 = Thread(target=func3)
    # t1.start()
    # t2.start()
    # t3.start()
    # t1.join()
    # t2.join()
    # t3.join()
    print("Total:",time.time()-start)