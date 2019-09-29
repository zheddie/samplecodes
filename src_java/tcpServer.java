import java.net.*;
import java.io.*;

public class tcpServer {
    
    static int port;
    ServerSocket server_socket;
    public static void main(String args[]) throws InterruptedException {
		try { 
		    port = Integer.parseInt(args[0]);
		}
		catch (Exception e) {
		    System.out.println("port = 12345 (default)");
		    port = 12345;
		}
		tcpServer t1 = new tcpServer();
		t1.startServer();
		
	
}
    void startServer() {
    	try {
			  server_socket = new ServerSocket(port, 100);
		 
		    while(true) {
		    	  System.out.println("Server waiting for client on port " + server_socket.getLocalPort());
		        Socket _socket = server_socket.accept();
		  	    //socket.setTcpNoDelay(true);
		        //socket.setSoLinger(true, 20);
		        //socket.setSoTimeout(5000);
		        System.out.println("A new request come in from " + _socket.getInetAddress() +":"+_socket.getPort());
		        new Thread(new handleSocket(_socket)).start();  
				}
			}
		catch(IOException e) {
		   e.printStackTrace();
	    }			
    }
		
    class handleSocket implements Runnable {  
        Socket client;  
        public handleSocket(Socket client) {  
            this.client = client;  
        }  
        public void run() {  
          int numOfBytes=0;
          byte[] buf= new byte[256];
          byte[] reply = new byte[50];

          for(int i=0; i<50; i++) {
              reply[i] = (byte)0x0;
          }
          reply[0] = (byte)0x10;
          reply[25] = (byte)0x20;
          try { 
          	 InputStream reader = client.getInputStream(); 
		    	   OutputStream output = client.getOutputStream(); 
             while(true) {
		         	        	
		             System.out.println("Receive request from client...");
		             int r = reader.read(buf,0,255);
		             System.out.println("Received " + r + " bytes");    
		             if(r <= 0) {
		             	break;
		             }       	
		             numOfBytes = numOfBytes + r;
		             if (numOfBytes == 25)
		              break;
		             
		          }  
		          if (numOfBytes == 25) {
              // Send reply 50 bytes
                System.out.println("Received request, send 50 bytes reply");
                output.write(reply); 
              } 
              else {
              
                System.out.println("Error occured, received " + numOfBytes + " bytes");
              }
               
		      }
		      catch(IOException e) {
		      	 e.printStackTrace();
		      }

        }  
    }  
}  