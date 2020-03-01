package zg.usa;

public class JNI_CALL1 {


	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		TestThread p2 = new TestThread("The second thread.");
		p2.start();
		System.out.println("In main thread after p2!"+Thread.currentThread().getId());
		TestThread p3 = new TestThread("The third thread.");
		p3.start();
		System.out.println("In main thread after p3!"+Thread.currentThread().getId());

 
	}

}
class TestThread extends Thread
{

	private String x ="Hello";
	public TestThread(String s){
		x = s;
	}
	public void run(){
	 
	 try {
		Thread.sleep(100);
	} catch (InterruptedException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}
	 String y = System.getProperty("java.home");
	 System.out.println(x+Thread.currentThread().getId()+"\njava.home="+y);
	  //  Here is the 'main' code of this class. This is what runs when you
	  //  enter 'java NativeHello' on the command line.

	    
	     //  Allocate a new NativeHello object now.
	     NativeHello nh = new NativeHello();
	 
	     //  Echo location.
	     System.out.println("(Java) Instantiated NativeHello object");
	     
	     System.out.println("(Java) string field is '" + nh.theString + "'");
	     System.out.println("(Java) Calling native method to set the string");
	 
	     //  Here is the call to the native method.
	     nh.setTheString();
	     nh.setTheString();
	     //  Now, print the value after the call to double check.
	     System.out.println("(Java) Returned from the native method");
	     System.out.println("(Java) string field is '" + nh.theString + "'");
	     System.out.println("(Java) All done...");  	 
  }
}
