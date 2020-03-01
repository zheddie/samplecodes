
public class zGCtest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String s = new String("xxxx");
		Integer i = new Integer(100);
		s = s+"yyyyyy";
		
		System.gc();
		System.out.println("s="+s);
	}

}
