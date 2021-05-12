//////////////////////////////////////////////////////////////////////////////////
//
// This example program loads the native method library 'PaseExample1'.
// The source code for the native method is contained in PaseExample1.c
// The printString method in this Java program uses a native method,
// getStringNative to retrieve the value of the String. The native method
// simply calls back into the getStringCallback method of this class.
//
//////////////////////////////////////////////////////////////////////////////////



import java.lang.*;
public class PaseExample1 {
    public synchronized void sleepawhile(){
		for(int i=0;i<15;i++){
	             try {
		        // thread to sleep for 1000 milliseconds
		        Thread.sleep(1000);
		        System.out.print(".");
		     } catch (Exception e) {
		        System.out.println(e);
		     }
		}
    }
    public static void main(String args[]) {
		System.out.println("-----");
		PaseExample1 pe1 = new PaseExample1("String for PaseExample1");
		pe1.printString();
		System.out.println("");
		System.out.println("-----");
    }

    String str;

    PaseExample1(String s) {
		str = s;
    }


    //-----------------------------------------------------------------
    public synchronized void printString() {
	Runtime.getRuntime().addShutdownHook(new Thread()
        {
            @Override
            public void run()
            {
		
				synchronized(this){
                	System.out.println("Shutdown hook ran!");
                	synchronized (str){ 
						String result = getStringNative();
						System.out.println("Value2 of str is '" + result + "'");
					}
				}
          	}
        });
	synchronized (str){ 
		String result = getStringNative();
		System.out.println("Value1 of str is '" + result + "'");
	}
    }

    // This calls getStringCallback through JNI.
    public native synchronized String getStringNative();

    // Called by getStringNative via JNI.
    public synchronized String getStringCallback() {
	sleepawhile();
	synchronized(str){
		str+="SYNC";
	}
	return str;
    }


    //-----------------------------------------------------------------
    static {
        System.out.println("librarypath="+System.getProperty("java.library.path"));
//	System.load("./libPaseExample1.so");
	// System.loadLibrary("PaseExample1");  //For PASE.
       System.loadLibrary("ZSIGSRV");		//For ILE.
	//System.loadLibrary("libPaseExample1.so");
    }

}
