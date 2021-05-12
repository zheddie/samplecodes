
class MyRunnable implements Runnable{
	myInt myj;
	void initInt(int s){
        myj = new myInt();
		myj.setInt(s);
	}
	@Override
	public void run() {
		for (int i = 0; i < 5; i++) {
			try{
				Thread.sleep(100);
			}catch(Exception e){
				e.printStackTrace();
			}
			//synchronized(myj){
				// System.out.println(Thread.currentThread().getName()+",myint="+myj.getInt());
				// myj.increaseInt();
			//}
			int toprint=myj.getInt_inc();
			System.out.println(Thread.currentThread().getName()+",myint="+toprint);
			
		}
	}
	public static void main(String [] argv){
        MyRunnable myRunnable = new MyRunnable();
        myRunnable.initInt(0);
		Thread t1 = new Thread(myRunnable);
		Thread t2 = new Thread(myRunnable);
		t1.start();
        t2.start();
        try{
            t1.join();t2.join();
        }catch(Exception e){
            e.printStackTrace();
        }
        System.out.println(myRunnable.myj.getInt());
    }
}
