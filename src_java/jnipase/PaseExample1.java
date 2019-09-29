//////////////////////////////////////////////////////////////////////////////////
//
// This example program loads the native method library 'PaseExample1'.
// The source code for the native method is contained in PaseExample1.c
// The printString method in this Java program uses a native method,
// getStringNative to retrieve the value of the String. The native method
// simply calls back into the getStringCallback method of this class.
//
//////////////////////////////////////////////////////////////////////////////////



public class PaseExample1 {
    public static void main(String args[]) {
	PaseExample1 pe1 = new PaseExample1("String for PaseExample1");
	pe1.printString();
    }

    String str;

    PaseExample1(String s) {
	str = s;
    }


    //-----------------------------------------------------------------
    public void printString() {
	String result = getStringNative();
	System.out.println("Value of str is '" + result + "'");
    }

    // This calls getStringCallback through JNI.
    public native String getStringNative();

    // Called by getStringNative via JNI.
    public String getStringCallback() {
	return str;
    }


    //-----------------------------------------------------------------
    static {
        System.out.println("librarypath="+System.getProperty("java.library.path"));
	System.loadLibrary("PaseExample1");
    }

}