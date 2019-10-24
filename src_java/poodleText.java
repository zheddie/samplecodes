import java.security.NoSuchAlgorithmException;

import javax.net.ssl.SSLContext;


public class poodleText {

	public static void main(String[] args) throws NoSuchAlgorithmException {
		// TODO Auto-generated method stub
		System.out.println("override: " + System.getProperty("com.ibm.jsse2.overrideDefaultProtocol"));
        System.setProperty("com.ibm.jsse2.overrideDefaultProtocol", args[0]);
        System.out.println("override: " + System.getProperty("com.ibm.jsse2.overrideDefaultProtocol"));
        SSLContext defcontext = SSLContext.getDefault();
        System.out.println("Provider: " + defcontext.getProvider()+ " : " + defcontext.getProtocol());
        String [] protocols = defcontext.getDefaultSSLParameters().getProtocols();
        for(int i = 0; i< protocols.length; i++)
        {
          System.out.println("protocols " + i + " : "+ protocols[i] );
        }  
	}

}
