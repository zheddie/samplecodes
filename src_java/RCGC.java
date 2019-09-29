public class RCGC{
	public Object instance = null;
	private static final int _1MB = 1024*1024;
	private byte[] bigSize = new byte[2* _1MB];
	public static void main(String [] args){
		RCGC objA = new RCGC();
		RCGC objB = new RCGC();
		objA.instance = objB;
		objB.instance = objA;
		objA = null;
		objB = null;
		//Oxxxx
		System.gc();
	}
}
