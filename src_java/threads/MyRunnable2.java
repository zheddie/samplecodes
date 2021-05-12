
class MyRunnable2 implements Runnable{

	@Override
	public void run() {
        int x[];
        x = new int[5];
        try{
            Thread.sleep(100);
            x[5]=0;
        }catch(Exception e){
            System.out.println("Thread:"+Thread.currentThread().getName());
            e.printStackTrace();
        }
	}
	public static void main(String [] argv){
        MyRunnable2 myRunnable = new MyRunnable2();
		Thread t1 = new Thread(myRunnable);
		Thread t2 = new Thread(myRunnable);
		t1.start();
        t2.start();
        try{
            t1.join();t2.join();
        }catch(Exception e){
            e.printStackTrace();
        }
        System.out.println("END");
    }
}
