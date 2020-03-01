import java.io.File;
import java.io.RandomAccessFile;
import java.util.*;
import java.text.*;

public class zgThreads{

	public static boolean jobend = false;
	public static final int DEFAULT_THREAD_NUMBER = 150;
	public static final int DEFAULT_RUNNING_SECONDS = 60;
	public static final int DEFAULT_CREATE_INTERVAL = 100;
	public static int inputCreators = 20;
	public static int inputThreads = DEFAULT_THREAD_NUMBER;
	public static int inputInterval = DEFAULT_CREATE_INTERVAL;
	public static int inputWaittime = DEFAULT_RUNNING_SECONDS;
	public static int inputHeapThread = 1;
	public static int inputHeapMonitor = 0;
	public static int createdTheads = 0;
	public static int endedTheads = 0;
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		for(int i = 0; i <args.length;i++){
			if (i == 0){
				inputCreators = Integer.valueOf(args[0]).intValue();
			}
			if (i == 1){
				inputThreads = Integer.valueOf(args[1]).intValue();
			}
			if (i == 2){
				inputHeapThread = Integer.valueOf(args[2]).intValue();
			}
			if (i == 3){
				inputHeapMonitor = Integer.valueOf(args[3]).intValue();
			}

			if (i == 4){
				inputInterval = Integer.valueOf(args[4]).intValue();
			}
			if (i == 5){
				inputWaittime = Integer.valueOf(args[5]).intValue();
			}
			
		}
/*		System.out.println(""+inputCreators
				+","+inputThreads
				+","+inputHeapThread
				+","+inputInterval
				+","+inputWaittime
				);*/
		zgThreads mainOBJ = new zgThreads();
		Thread monitor = new Thread(new MonitorThread());
		
		System.out.println("zgjava.MAIN.startTime:"+Calendar.getInstance().getTimeInMillis()/1000+"secs"
				+",creators="+mainOBJ.inputCreators+",threads="+zgThreads.inputThreads
				+",HeapAllocate4Thread(10k)="+inputHeapThread
				+",HeapAllocate4Monitor(5k)="+inputHeapMonitor
				+",speed(n/s)="+1000/zgThreads.inputInterval
				+",waittime(secs)="+zgThreads.inputWaittime);
		monitor.start();
		mainOBJ.StartCreatThreads();
		System.out.println("zgjava.MAIN.  endTime:"+Calendar.getInstance().getTimeInMillis()/1000+"secs");
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
	private int startedThreadCount;
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

		startedThreadCount = 0;

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
				zgThreads.createdTheads ++;
				//if(error != ""){
				/*System.out.println("zgjava.CREATOR,"+"creator:"+Thread.currentThread().getId()+",ThreadIndex:"+i
						+",ThreadID="+threadArray[i].getId()
						+",createTime(secs)="+initTime/1000
						+","+error);*/
				//}
				char[][] sc = new char[(int)tt][10];
				//start a thread and wait for it to finish
				threadArray[i].start();				
				Thread.sleep(zgThreads.inputInterval);
				synchronized (this) {
					++startedThreadCount;
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
		int cT = 0;
		int eT = 0;
		long initT = Calendar.getInstance().getTimeInMillis();
		int index = 0;
		while(true){
			
			try{
				for(int i = 0;i<5;i++){
					if(zgThreads.inputHeapMonitor > 0){
						char[][] sc = new char[1024][zgThreads.inputHeapMonitor];
					}
					Thread.sleep(1000);
					System.out.print("^");
				}
			}
			catch(Exception e){
				e.printStackTrace();
			}
			long currT = Calendar.getInstance().getTimeInMillis();
			long diff = (currT - initT)/1000;
			int crT = zgThreads.createdTheads;
			int enT = zgThreads.endedTheads;
			cT = crT - cT;
			eT = enT - eT;
			initT = Calendar.getInstance().getTimeInMillis();
			if(diff > 6){
				System.out.println("zgjava.ERROR!!!!diff="+diff
						+",currentTime(secs)="+initT/1000
						+",createdThreads="+cT+"("+crT+")"
						+",endedThreads="+eT+"("+enT+")"
						+",index="+index);
			}else{
				System.out.println("zgjava.TIMESTAMP" +
						",currentTime(secs)="+initT/1000
						+",createdThreads="+cT+"("+crT+")"
						+",endedThreads="+eT+"("+enT+")"
						+",index="+index);
			}
			cT = crT;
			eT = enT;
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
		return Calendar.getInstance().getTimeInMillis();
	}
	public void run() {
		//do some random things
		int ST = (int)sleepTime/10;
		
		//sleep for several seconds
		try {
			
			for(int i =0;i<10;i++){
				if(zgThreads.inputHeapThread == 0) zgThreads.inputHeapThread = 1;
				char[][] ch = new char[1024][zgThreads.inputHeapThread];
				Thread.sleep(ST);
			}
			zgThreads.endedTheads ++;
			long currentTime = this.GetMilliTime();
			long timeUsed = currentTime - startTime;
			//We expect diff = sleepTime here.
			long diff = (timeUsed-sleepTime)/1000;
			//if(diff> 30){
				/*System.out.println("zgjava,THREAD.END.creator:"+threadNumber
						+",ThreadID="+Thread.currentThread().getId()
						+",currentTime="+currentTime/1000
						+",diff="+diff+",createTime(secs)="+startTime/1000+" slept "
						+ timeUsed/1000 + "secs. Expect:"+ sleepTime/1000 +"secs.");
						*/
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


