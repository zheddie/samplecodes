import java.net.*;
import java.io.*;
import java.lang.*;

public class tcpClient implements Runnable{   
    
    int port =12345;
    String _server;
    Socket _socket = null;
    String threadName;
    
    int timeout =60000;
    Thread client;
    static int threadCount=1;
    
    tcpClient() {
        threadName = new String("TcpClient" + (threadCount++));
        client = new Thread(this, threadName);
        
    }
    
   	public void startThread(String currentServer, int currentPort) throws IOException {

        _server = currentServer;
        port =currentPort;
        //socket = current;
       // inStream_ = socket.getInputStream();
       // outStream_ = socket.getOutputStream();
        
        
        //client.setDaemon(true);
        //client.setDaemon(true);
        client.start();

    }
    
    
    public void run() {
		    System.out.println(threadName + " Connect to server port "+ port);
		    try {
		    	_socket = new Socket(_server, port);
		    	_socket.setSoTimeout(timeout);
		    }
		    catch (UnknownHostException e) {
		        System.out.println(e);
		        System.exit(1);
		    }
		    catch (IOException e) {
		        System.out.println(e);
		        System.exit(1);
		    }
		    
		    byte buf[] = new byte[25];		    	
		    byte[] request = new byte[25];
		    byte[] reply =new byte[100];
        int numOfBytes=0;
		    try {
		    	InputStream reader = _socket.getInputStream(); 
		    	OutputStream output = _socket.getOutputStream();
		
		    	//client initially sends 50 bytes data
		     	output.write(request);
		      //System.out.println(threadName + " Read from local port " + _socket.getLocalPort());	   
		       while(true) {
		        	int r = reader.read(buf, 0, 25);
		          if(r <= 0 ) {
		          	//System.out.println(threadName+ " reader.read return r = " + r);
		        		break;
		        	}
		        	
		        	for(int i= 0 ; i <  r; i++) {
		        	   reply[numOfBytes + i] = buf[i];
		        	}
		        	numOfBytes = numOfBytes + r;
		        	System.out.println(threadName + " " + "Received " + numOfBytes + " bytes");        	
		    	}
		    }catch (SocketTimeoutException e) { 
		        e.printStackTrace();
		    	  if(numOfBytes < 50 && numOfBytes >=0) {	  
		    	      System.out.println(threadName + " " + "missing bytes, "+ "received "+ numOfBytes+ " bytes" );  
		    	  } else {
		    	      if(reply[0] == 0x10 && reply[25] == 0x20 ) {
		    	           System.out.println(threadName +  " " + "received correct reply");  
		    	      }
		    	      else {
		    	           System.out.println(threadName +  " " +"received wrong reply "+ "reply[0]="+reply[0]+ " reply[25]=" +reply[25]);     
		    	      }
		    	    }
		     }catch (IOException e) {
		    	 e.printStackTrace();
		    }
           
        
       	if(numOfBytes < 50 && numOfBytes >=0) {	  
		    	      System.out.println(threadName + " " + "missing bytes, "+ "received "+ numOfBytes+ "bytes" );  
		    } else 
		    { 
		    	      if(reply[0] == 0x10 && reply[25] == 0x20 ) {
		    	           System.out.println(threadName +  " " + "received correct reply");  
		    	      }
		    	      else {
		    	           System.out.println(threadName +  " " +"received wrong reply "+ "reply[0]="+reply[0]+ " reply[25]=" +reply[25]);   
		    	      }
		   }
        
    }
    public void forceDisconnect() {
		   // client.interrupt(); 
		   //System.out.println(threadName + ">>>>   interrupt");   
		   try {
		    //	_socket.shutdownInput();
		    //	_socket.shutdownOutput();
		    	_socket.close();
		    } catch (Exception e) {

		        e.printStackTrace();
        } 
        try  {
            //OutputStream output = test.c1[r].getSocket().getOutputStream();
            OutputStream output =_socket.getOutputStream();
		     	  
		     	  output.write((new String("hello")).getBytes());
		     	  }
		     	  catch(Exception e)
		     	  {
		     	  	 e.printStackTrace();
		     	  }        
//        try
//        {
//         client.join();
//        }
//        catch (InterruptedException e)
//        {
//          System.out.println(threadName);
//          e.printStackTrace();                             
//        }

    }
          
    public String getName() {
        return client.getName();
    }   
    public Boolean isAlive() {
        return client.isAlive();
    }   
    public Socket getSocket() {
        return _socket;
    }
}
