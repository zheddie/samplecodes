
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.InputStream;

public class RuntimeDemo {

   public static void main(String[] args) {
      try {

      // create a new array of cmd
     	 String[] cmdArray = new String[]{args[0],"-Xgc:suballocatorCommitSize=300m","-version"};

      // print a message
     	 System.out.println("Executing java command -Xgc:suballocatorCommitSize=300m.\n");

     // create a process and execute cmdArray and currect environment
      	Process process = Runtime.getRuntime().exec(cmdArray);
	printMessage(process.getInputStream());
	printMessage(process.getErrorStream());
        Thread.sleep(10000);  //sleep seconds to give more time to read the stdout and stderr
        
	int value = process.waitFor();
     // print complete message
      	System.out.println("The runtime.exec completed with value "+value);

      } catch (Exception ex) {
         ex.printStackTrace();
      }
   }

  private static void printMessage(final InputStream input) {
    new Thread(new Runnable() {
       public void run() {
		BufferedReader bf = new BufferedReader(new InputStreamReader(input));
		String line = null;
		try {
			while((line=bf.readLine())!=null) {
				System.out.println(line);
			}		
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	}).start();
    }
}
