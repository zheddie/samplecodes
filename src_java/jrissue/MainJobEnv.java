import java.io.*;
 
public class MainJobEnv
{
   public static void main(String args[])
   {
      Process theProcess = null;
      BufferedReader inStream = null;
      //System.out.println("CallHelloPgm.main() invoked");
 
      // call the Hello class
      try
      {
         theProcess = Runtime.getRuntime().exec("system -e QSYS/call  PGM(GAVIN/ILEPRINT)");

        //theProcess = Runtime.getRuntime().exec("ls");
		 //theProcess = Runtime.getRuntime().exec("java mySleep");
         System.out.println("zg.after exec");
      }
      catch(IOException e)
      {
         System.err.println("Error on exec() method");
         e.printStackTrace();  
      }
        
      // read from the called program's standard output stream
      try
      {

		InputStream in = theProcess.getInputStream();
		System.out.println("zg.after getInputStream");
		int result = 0;
		String Unicode = "";
		byte [] inputData = new byte[30];
		while (true) {
			result = in.read(inputData);
            System.out.println("zg.after read");
			int len = inputData.length;
			System.out.println(len);
			try{
                System.out.println("Sleep 2 seconds here.");
				Thread.sleep(2000);
			}
			catch(InterruptedException e)
			{
				System.err.println("Error on sleep");
				e.printStackTrace();  
			}
			
			if (result < 0)
			{
				
				break;
			}
			else {
				System.out.println("result: " + result );
				for (int i = 0; i < len; i++) {
					Unicode += "\\u"+Integer.toHexString(inputData[i] & 0X00FF);	
				}
				System.out.println(Unicode);
			}
        }

	  }
      catch(IOException e)
      {
         System.err.println("Error on inStream.readLine()");
         e.printStackTrace();  
      }
   } 
} 