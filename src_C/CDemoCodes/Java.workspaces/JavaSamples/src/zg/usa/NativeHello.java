package zg.usa;

public class NativeHello {
	 
	  //  Declare a field of type 'String' in the NativeHello object.
	  //  This is an 'instance' field, so every NativeHello object 
	  //  contains one.
	  public String theString;           // instance variable
	  //  Declare the native method itself.  This native method 
	  //  creates a new string object, and places a reference to it 
	  //  into 'theString'
	  public native void setTheString(); // native method to set string		 
	  //  This 'static initializer' code is called before the class is
	  //  first used.
	  static {
	     //  Attempt to load the native method library.  If you do not
	     //  find it, write a message to 'out', and try a hardcoded path.
	     //  If that fails, then exit.
	     try {
	        //  System.loadLibrary uses the java.library.path property or 
	        //  the LIBPATH environment variable.
	        System.loadLibrary("NATHELLO");
	     }
	     catch (UnsatisfiedLinkError e1) {
	        //  Did not find the service program.
	        System.out.println("I did not find NATHELLO *SRVPGM.");
	        System.out.println("(I will try a hardcoded path)");
	 
	        try {
	           //  System.load takes the full integrated file system form path.
	           System.load ("/qsys.lib/gavin.lib/nathello.srvpgm");
	        }
	        catch (UnsatisfiedLinkError e2) {
	           //  If you get to this point, then you are done!  Write the message
	           //  and exit.
	           System.out.println
	              ("<sigh> I did not find NATHELLO *SRVPGM anywhere. Goodbye");
	           System.exit(1);
	        }
	     }
	  }
		public static void main(String[] args) {
			// TODO Auto-generated method stub
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
			 
			     //  Now, print the value after the call to double check.
			     System.out.println("(Java) Returned from the native method");
			     System.out.println("(Java) string field is '" + nh.theString + "'");
			     System.out.println("(Java) All done...");   
		}


	}