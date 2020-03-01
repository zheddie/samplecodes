package zg.usa;

public class LongSleep {

	/**
	 * @param args
	 * @throws InterruptedException 
	 */
	public static void main(String[] args) throws InterruptedException {
		// TODO Auto-generated method stub
		int i = 1;
		String count;
		if (args.length > 0) {
			count = args[0];
			System.out.println("Args[1]="+count);
			i = Integer.parseInt(count).intValue();
		}
		
		System.out.println("BeforeSleep");
		Thread.sleep(1000* i);
		System.out.println("AfterSleep");
	}

}
