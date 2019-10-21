public class VolatileTest{
	public static int race = 0;
	public static void increase(){
		race++;
	}
	private static final int THREADS_COUNT=20;
	public static void main (String[] args){
		Thread [] threads = new Thread[THREADS_COUNT];
		for (int i =0;i<THREADS_COUNT; i++){
			threads[i] = new Thread( new Runnable(){
				@Override
				public void run(){
					for (int i=0;i<1000;i++){
						increase();
					}
					System.out.println("thread run done.");
				}
			});
			threads[i].start();
			System.out.println("thread:"+i+" created!");
		}
		while(Thread.activeCount()>2){
			System.out.println("Thread.activeCount="+Thread.activeCount());
			Thread.yield();
		}
		System.out.println("race="+race);
	}
}
