
public class JNITest {

	public native void nativeOne();
	static {
		System.loadLibrary("NativeLib");
	}
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		JNITest x = new JNITest();
		x.nativeOne();
	}

}
