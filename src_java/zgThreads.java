import java.util.*;

public class zgThreads{

	public static boolean jobend = false;
	public static final int DEFAULT_THREAD_NUMBER = 150;
	public static final int DEFAULT_RUNNING_SECONDS = 60;
	public static final int DEFAULT_CREATE_INTERVAL = 100;
	public static int inputCreators = 20;
	public static int inputThreads = DEFAULT_THREAD_NUMBER;
	public static int inputInterval = DEFAULT_CREATE_INTERVAL;
	public static int inputWaittime = DEFAULT_RUNNING_SECONDS;
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		int arglen = args.length;

		if (arglen > 0){
			inputCreators = Integer.valueOf(args[0]).intValue();
		}
		if (arglen> 1){
			inputThreads = Integer.valueOf(args[1]).intValue();
		}
		if (arglen> 2){
			inputInterval = Integer.valueOf(args[2]).intValue();
		}
		if (arglen> 3){
			inputWaittime = Integer.valueOf(args[3]).intValue();
		}
		
		System.out.println(""+inputCreators
				+","+inputThreads
				+","+inputInterval
				+","+inputWaittime
				);
		zgThreads mainOBJ = new zgThreads();
		Thread monitor = new Thread(new MonitorThread());
		
		System.out.println("zgjava.MAIN.startTime:"+System.currentTimeMillis()/1000+"secs"
				+",creators="+zgThreads.inputCreators+",threads="+zgThreads.inputThreads
				+",speed(n/s)="+1000/zgThreads.inputInterval
				+",waittime(secs)="+zgThreads.inputWaittime);
		monitor.start();
		mainOBJ.StartCreatThreads();
		System.out.println("zgjava.MAIN.  endTime:"+System.currentTimeMillis()/1000+"secs");
		jobend = true;
		
	}
	private void StartCreatThreads(){
		
		Thread[] ct = new Thread[inputCreators];
		try{
			for(int i = 0 ;i<inputCreators;i++){
				ct[i] = new Thread( new CreateThreads());
				ct[i].start();
			}
			for(int j=0;j<inputCreators;j++){
				ct[j].join();
			}
		}
		catch(Exception e){
			e.printStackTrace();
		}
	}
}
class CreateThreads implements Runnable{

	

	private Random randomNumber;
	private int threadNo;
	private int runningSeconds;
	/**
	 * Get system time in milliseconds
	 * 
	 * @return
	 */
	private long GetMilliTime() {
		return Calendar.getInstance().getTimeInMillis();
	}



	private void InitializeParameters(int nNum, int nSeconds) {
		threadNo = nNum;
		runningSeconds = nSeconds;

		randomNumber = new Random(System.currentTimeMillis());
	}


	// This function uses main thread.
	private void StartThreads() {
		int i;
		Thread[] threadArray = new Thread[threadNo];
		String error = new String(new char[256]);
		try{
			long initTime = GetMilliTime();
			for (i = 0; i < threadNo; i++) {
				long tt = Math.abs(randomNumber.nextLong()) % (runningSeconds*1000);
				//System.out.print("zgjava.ThreadID:"+i+" in creating.");
				// every created thread sleep for 0 ~ runningSeconds seconds.
				long CurrentT = GetMilliTime()/1000;
				long CountTime =(zgThreads.inputInterval+initTime)/1000;
				initTime = GetMilliTime();//Get the current immediately.
				if(CurrentT-CountTime >10){//Over 1 seconds are used for thread create. ERROR. 
					error = "ERR:diff="+(CurrentT-CountTime)+",currentTime="+CurrentT+"secs,Expected:"+CountTime+"secs.";
				}
				else {
					error = "";
				}
				threadArray[i] = new Thread(new ThreadClass(Thread.currentThread().getId(), tt,initTime));
				//if(error != ""){
//				System.out.println("zgjava.CREATOR,"+"creator:"+Thread.currentThread().getId()+",ThreadIndex:"+i
//						+",ThreadID="+threadArray[i].getId()
//						+",createTime(secs)="+initTime/1000
//						+","+error);
				//}
				char[][] sc = new char[(int)tt][100];
				//start a thread and wait for it to finish
				threadArray[i].start();				
				Thread.sleep(zgThreads.inputInterval);
				synchronized (this) {
				}
			}
			
			for (i = 0; i < threadNo; i++) {
				threadArray[i].join();			
			}
			
		}catch (Exception e) {
			e.printStackTrace();
			System.out.println("zgjava.Creating threads interrupted!");
		}


	}	
	public void run(){
		this.InitializeParameters(zgThreads.inputThreads, zgThreads.inputWaittime);
		
		this.StartThreads();
	}
}
class MonitorThread implements Runnable{
	public void run(){
		long initT = Calendar.getInstance().getTimeInMillis();
		int index = 0;
		while(true){
			try{
				Thread.sleep(5*1000);
			}
			catch(Exception e){
				e.printStackTrace();
			}
			long currT = Calendar.getInstance().getTimeInMillis();
			long diff = (currT - initT)/1000;
			initT = Calendar.getInstance().getTimeInMillis();
			if(diff > 5){
				System.out.println("zgjava.ERROR!!!!diff="+diff
						+",currentTime(secs)="+initT/1000
						+",index="+index);
			}else{
//				System.out.println("zgjava.TIMESTAMP" +
//						",currentTime(secs)="+initT/1000
//						+",index="+index);
			}
			index++;
			if(zgThreads.jobend) break;
		}
		
	}
}
class ThreadClass implements Runnable {
	
	
	private long threadNumber;
	private long sleepTime;
	private long startTime;
	
	public ThreadClass(long no, long theTime,long createTime) {
		threadNumber = no;
		sleepTime = theTime;
		
		startTime = createTime;
	}

	private long GetMilliTime() {
		return System.currentTimeMillis();
	}
	public void run() {
		//do some random things
		int ST = (int)sleepTime/100;
		
		//sleep for several seconds
		try {
			for(int i =0;i<100;i++){
				char[][] ch = new char[516+258][ST];
				Thread.sleep(ST);
			}
			long currentTime = this.GetMilliTime();
			long timeUsed = currentTime - startTime;
			//We expect diff = sleepTime here.
			long diff = (timeUsed-sleepTime)/1000;
			//if(diff> 30){
//				System.out.println("zgjava,THREAD.END.creator:"+threadNumber
//						+",ThreadID="+Thread.currentThread().getId()
//						+",currentTime="+currentTime/1000
//						+",diff="+diff+",createTime(secs)="+startTime/1000+" slept "
//						+ timeUsed/1000 + "secs. Expect:"+ sleepTime/1000 +"secs.");
			//}
			//long total = threadNumber * zgThreads.inputInterval +sleepTime;
			//System.out.println("zgjava.------------------------------- Thread " + Thread.currentThread().getId() + ": \thas slept for "
				//	+ sleepTime + " milliseconds." +"TOTAL:"+ total );
		} catch (Exception e) {
			System.out.println("zgjava.Exception!!!");
			e.printStackTrace();
		}
	}
}


