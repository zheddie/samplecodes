import java.lang.*;
import java.io.*;

public class SystemDemo {

   public static void main(String[] args) throws Exception {
    
      // create file
//      FileOutputStream f = new FileOutputStream("file.txt");
	PrintStream fileOut = new PrintStream("file2.txt");
      System.setOut(fileOut);

      // this text will get redirected to file
      System.out.println("This is System class!!!");
   }
}
