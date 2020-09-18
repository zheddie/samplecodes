import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;

import javax.net.ssl.*;
import java.security.Provider;

public class SSLContextTest {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		SSLContext context;
		try {
      Provider provider = SSLContext.getDefault().getProvider();
      System.out.println(provider.getClass().getName());
			//context = SSLContext.getInstance("TLS");
			context = SSLContext.getDefault();
			//context.init(null,null,null);
			SSLSocketFactory factory = (SSLSocketFactory)context.getSocketFactory();
			SSLSocket socket = (SSLSocket)factory.createSocket();
			String[] protocols = socket.getSupportedProtocols();
			String supported = "Supported Protocols: ";
			for (int i = 0; i < protocols.length; i++)
			   supported += " " + protocols[i];

			protocols = socket.getEnabledProtocols();
			String enabled = "Enabled Protocols: ";
			for(int i = 0; i < protocols.length; i++)
			   enabled += " " + protocols[i];
			    
			System.out.println(supported + "\n\n" + enabled);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}		
	}
}
