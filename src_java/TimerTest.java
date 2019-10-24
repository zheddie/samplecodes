import java.io.IOException;
import java.util.Calendar;
import java.util.Date;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;


public class TimerTest {
	
	private class TimerThread implements Runnable{
		private int interval=1000;
		public TimerThread(int intval) {
			super();
			this.interval=intval;
		}

		public void run(){
			long startTime=0,endTime=0;
			long dif=0;
			while (true) {
				startTime=System.currentTimeMillis();
				try {
					Thread.sleep(interval);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
				endTime=System.currentTimeMillis();
				dif=endTime-startTime;
				if(dif>1200000) {
					System.out.println(Thread.currentThread().getName()+": startTime="+startTime+", endtime="+endTime+", dif="+dif+", current time="+Calendar.getInstance().getTime() );
				}
			}
		}
	}
	
	public void runTest(String threadNO, String interval){
	      int threadNumber=Integer.parseInt(threadNO);
		    int intval=Integer.parseInt(interval);
			ExecutorService es = Executors.newCachedThreadPool();
			for (int j = 0; j < threadNumber; j++) {
				es.execute(new TimerThread(intval));
			}
			es.shutdown();
			try {
				if (!es.awaitTermination(3, TimeUnit.HOURS)) {
					System.out
							.println("Thread pool didn't terminate in 3 hours");
				}
			} catch (InterruptedException e1) {
				e1.printStackTrace();
			}
	}
	
	public static void main(String[] args) {
		if(args.length==2){
			TimerTest t=new TimerTest();
			System.out.println("*********Start at:"+(new Date())+"********************");
			t.runTest(args[0], args[1]);
		}else {
			System.out.println("Incorrect input. Usage: java TimerTest thread_number sleep_interval");
		}
	}

}
