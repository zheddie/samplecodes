
public class zgThread01 implements Runnable{

	/**
	 * @param args
	 */
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		zgThread01 zth = new zgThread01();
		Thread z01 = new Thread(zth);
		z01.start();
		long tID = Thread.currentThread().getId();
		System.out.println("In main!tID="+tID);
	}

	@Override
	public void run() {
		// TODO Auto-generated method stub
		long tID = Thread.currentThread().getId();
		System.out.println("In child!tID="+tID);
	}

}
