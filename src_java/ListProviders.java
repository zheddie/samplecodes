import java.security.Provider;
import java.security.Security;

public class ListProviders {

	public static void main(String[] args)
    {
     try
        {

            Provider[] P = Security.getProviders();
               int n = P.length;
               System.out.println("There are " + n + " providers");
               System.out.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
               for(int i = 0; i<n; i++)
               {
                 System.out.println("Provider " +(i+1)+ " is " + P[i].getName());
                 System.out.println(P[i].getVersion());
                 System.out.println(P[i].getInfo());
                 System.out.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
               }
            
            
            System.out.println("There are " + n + " providers");
            System.out.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
            for(int i = 0; i<n; i++)
            {
              System.out.println("Provider " +(i+1)+ " is " + P[i].getName());
              System.out.println(P[i].getVersion());
              System.out.println(P[i].getInfo());
              System.out.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
            }   
        
        }
        	catch (Throwable t)
            {
        		t.printStackTrace();
            }
    }
}