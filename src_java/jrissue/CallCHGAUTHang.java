import java.io.*;
 
public class CallCHGAUTHang
{
   public static void main(String args[])
   {
      Process theProcess = null;
      BufferedReader inStream = null;
      //System.out.println("CallHelloPgm.main() invoked");
 
      // call the Hello class
      try
      {
         theProcess = Runtime.getRuntime().exec("system QSYS/CHGAUT USER(*PUBLIC) OBJ('/home/bjjrwang/cps/ARE/areout.txt') DTAAUT(*EXCLUDE) OBJAUT(*NONE) SUBTREE(*NONE) SYMLNK(*NO)");
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
        /* inStream = new BufferedReader(
                               new InputStreamReader( theProcess.getInputStream() ));  
         //System.out.println(inStream.readLine());
		
			String line = "";
			while ((line = inStream.readLine()) != null) {
				System.out.println(line);
				  
  
			char[] chars = "0123456789ABCDEF".toCharArray();  
			StringBuilder sb = new StringBuilder("");  
			byte[] bs = line.getBytes();  
			int bit;  
			for (int i = 0; i < bs.length; i++) {  
				bit = (bs[i] & 0x0f0) >> 4;  
				sb.append("\\u"+chars[bit]);  
				bit = bs[i] & 0x0f;  
				sb.append(chars[bit]);  
			}  
 
			System.out.println(  bs.length); 
			System.out.println( sb.toString());  
 
			}*/
		
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
				Thread.sleep(10000);
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
		/*
		System.out.println("out of while...");
		System.out.println(result);
		int len = inputData.length;
		System.out.println(len);
		Unicode = "";
			for (int i = 0; i < len; i++) {
            Unicode += "\\u"+Integer.toHexString(inputData[i] & 0X00FF);	
		}
		System.out.println(Unicode);*/
		
		
		/*InputStream in = theProcess.getInputStream();
		int retB = 0;
		int ava = 0;
		String Unicode = "";
		while (in != null)
		{
			ava = in.available();
			System.out.println(ava);
			retB = in.read();
			if (retB < 0 )
			{
				
				System.out.println("will break");
				Unicode = "\\u"+Integer.toHexString(retB);
				System.out.println("The char is: "+retB +"\t"+ "The Unicode is:" + Unicode);
				break;
			}
			else{
				Unicode = "\\u"+Integer.toHexString(retB);
				System.out.println("The char is: "+retB +"\t"+ "The Unicode is:" + Unicode);
			}
		}*/
		
	  }
      catch(IOException e)
      {
         System.err.println("Error on inStream.readLine()");
         e.printStackTrace();  
      }
   } 
} 