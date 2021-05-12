import java.lang.*;
public class callILENative{
	public static void main(String args[]){
		callILENative cin = new callILENative();
		cin.printString();
		System.out.println("printString done.");
	}
	public native String printString();
    //-----------------------------------------------------------------
    static {
        System.out.println("librarypath="+System.getProperty("java.library.path"));
       System.loadLibrary("ZILENAT");		//For ILE.
    }
}