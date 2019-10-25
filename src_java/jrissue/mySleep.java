import java.io.*;
 
public class mySleep
{
   public static void main(String args[])
   {
      try{
        Thread.sleep(1000*10);
      }
      catch(InterruptedException e)
      {
        System.err.println("Error on sleep");
        e.printStackTrace();  
      }
      System.out.println("From mySleep.");
   } 
} 