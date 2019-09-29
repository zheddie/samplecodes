//////////////////////////////////////////////////////////////////////////////////
//
// This example program loads the native method library 'FileCheck'.
// The source code for the native method is contained in FileCheck.c
// The printString method in this Java program uses a native method,
// getStringNative to retrieve the value of the String. The native method
// simply calls back into the getStringCallback method of this class.
//
//////////////////////////////////////////////////////////////////////////////////

import java.io.*;


public class FileCheck {
    public static void main(String args[]) {
        FileCheck fc = new FileCheck();
        if(args.length <1){
                System.out.println("Usage:\n\t FileCheck <filename>\n");
                return;
        }
        String filename = args[0];
        File f = new File(filename);
        // new java.util.Scanner(System.in).nextLine();
        // try{
        //     System.in.read();
        // }catch(Exception e){
        //     e.printStackTrace();
        // }
        int rv = fc.getBooleanAttributes1(f);
        System.out.println("Return:rv="+rv);
    }

    public native int getBooleanAttributes1(File f);
    private static native void initIDs1();

    static {
        System.loadLibrary("zgfile");
        initIDs1();
    }


}