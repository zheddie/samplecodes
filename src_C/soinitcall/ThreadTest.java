
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.lang.reflect.Field;
import java.net.HttpURLConnection;
import java.net.URL;
import java.security.Provider;
import java.security.Security;
import java.util.concurrent.SynchronousQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;



public class ThreadTest {
    private static int threadNum = 50;

    public static void main(String[] args) {

    	generate();

    }

    public  static void generate() {
		String var[]= new String[]{"java","QIBMHello"};
    	for (int i=1; i<threadNum; i++) {
    		Forker c = new Forker(("Forker-" + i), var);
    	    c.start();

    		Dumper t = new Dumper( "Dumper-" + i);

    	    t.start();

        }
    }
}

final class Dumper implements Runnable{
    String mytitle;
    private Thread t;
    Dumper(String title){
       mytitle=title;
    }

	@Override
	public void run(){
		try {
		    Provider[] providers = Security.getProviders();
		    for (int i = 0; i < providers.length; i++) {
		        System.out.println("\nProvider #" + (i + 1) + ":");
		        System.out.println("\tName: " + providers[i].getName());
		    }
	    } catch(Exception e) {
	    	    e.printStackTrace();
	    }
	}

	  public void start() {
	      System.out.println("Starting " +  mytitle );
	      if (t == null) {
	         t = new Thread (this, mytitle);
	         t.start ();
	      }
	  }
}


final class Connector implements Runnable {
	String mytitle;
	Thread t;
    Connector(String title){

        mytitle=title;
    }


	public void run(){
		try {
            String url = new String("https://cn.bing.com/");
			URL u = new URL(url.trim());
			HttpURLConnection huc =  (HttpURLConnection)u.openConnection();
			huc.setRequestMethod("GET");
			huc.connect();
			System.out.println("Response code is "+ huc.getResponseCode());
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	  public void start() {
	      System.out.println("Starting " +  mytitle );
	      if (t == null) {
	         t = new Thread (this, mytitle);
	         t.start ();
	      }
	  }
}

class Forker implements Runnable{
	String mytitle;
    private String commands[];
    ProcessBuilder pb;
    Thread t;
    Forker(String title, String args[]){
    	mytitle = title;
    	commands=args;
    	Thread t=null;
        //System.out.println("args: "+commands[0]);
    }
    Forker(ProcessBuilder pb_in, String args[]){
    	    pb=pb_in;
    	    commands=args;
    	    System.out.println("args: "+commands[0]);
	}
    /* (non-Javadoc)
	 * @see java.lang.Runnable#run()
	 */
	@Override
	public void run(){
		try {
			ProcessBuilder pb = new ProcessBuilder();
            pb.command(commands);
			pb.environment().put("QIBM_USE_DESCRIPTOR_STDIO","Y");
			pb.redirectErrorStream(true);
			Process p = pb.start();
	        Field f = p.getClass().getDeclaredField("pid");
	        f.setAccessible(true);
	        int pid = f.getInt(p);
	        System.out.println("Process-"+pid);
	        BufferedReader bufferedReader =
	        		new BufferedReader(new InputStreamReader(new BufferedInputStream(p.getInputStream())));
	        String line;
	        while ((line = bufferedReader.readLine())!=null){
	        	    System.out.println(line);
	        }
	        bufferedReader.close();
	    } catch(Exception e) {
	    	    e.printStackTrace();
	    }
	}
	  public void start() {
	      System.out.println("Starting " +  mytitle );
	      if (t == null) {
	         t = new Thread (this, mytitle);
	         t.start ();
	      }
	  }
}
