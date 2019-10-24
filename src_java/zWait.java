
public class zWait {

	/**
	 * @param args
	 * @throws InterruptedException 
	 */
	public int age;
	
	public static void main(String[] args) throws InterruptedException {
		zWait zw = new zWait();
		zw.age = 10;
		zw.setName("HelloName");
		zw.mywait(args[0]);
		// TODO Auto-generated method stub
	}
	public void setName(String s){
	}
	public void mywait(String s){
		Integer iT = new Integer(s);
		System.out.println("In waiting....");
		synchronized(this){
		try{
			this.wait(1000*60*iT.intValue());
		}catch ( Exception e){
			e.printStackTrace();
		}
		}
		System.out.println("Endding...");
	
	}
}
