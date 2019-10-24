
public class javaSleep {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		long TM0 = System.currentTimeMillis();
		try {
			Thread.sleep(0, 40);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		long diff = System.currentTimeMillis() - TM0;
		System.out.println("diff="+diff);
		
	}

}
