
public class zSleep {

	/**
	 * @param args
	 * @throws InterruptedException 
	 */
	public static void main(String[] args) throws InterruptedException {
		zSleep zw = new zSleep();
		if(args.length> 0){
			zw.mywait(args[0]);
		}else{
			zw.mywait("3");
		}
		// TODO Auto-generated method stub
	}

	public void mywait(String s){
		int iT = Integer.parseInt(s);
		System.out.println("In waiting....");
		try{
			for(int x=0;x<iT*10;x++){
				Thread.sleep(1000*6);
				System.out.println(x);
			}
		}catch ( Exception e){
			e.printStackTrace();
		}
		System.out.println("Endding...");
	
	}
}
