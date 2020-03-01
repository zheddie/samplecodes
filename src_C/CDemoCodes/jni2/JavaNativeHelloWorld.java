public class JavaNativeHelloWorld
{
    // Load the Library
    static
    {
	String jcp = System.getProperty("java.library.path");
	System.out.println("java.library.path="+jcp);

        System.loadLibrary("helloworld");
//        System.load("/home/zhanggan/mydata/codes/jni2/libhelloworld.so");
    }
 
    // Native Functions
    public native void Nhelloworld();
 
    //Main Entry point
    public static void main(String[] args)
    {
        JavaNativeHelloWorld hw = new JavaNativeHelloWorld();
 
        hw.Nhelloworld();
    }
 
    
}
