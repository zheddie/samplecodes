
public class zWaitFPE {

	/**
	 * @param args
	 * @throws InterruptedException 
	 */
	public int age;
	public double len;
	public static void main(String[] args) throws InterruptedException {
		zWaitFPE zw = new zWaitFPE();
		zw.age = 10;
		zw.len = 172.5;
		double xx = zw.len/3.0;
		zw.setName("HelloName");
		zw.mywait(args[0]);
		System.out.println("xx="+xx);
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