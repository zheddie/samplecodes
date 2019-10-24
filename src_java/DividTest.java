
public class DividTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		long tID = Thread.currentThread().getId();
		
		System.out.println("equal?"+(tID == Integer.valueOf("2").longValue()));
	}

}
