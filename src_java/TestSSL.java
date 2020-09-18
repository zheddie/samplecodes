
import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.net.URLConnection;
import javax.net.ssl.*;
 
/**
  *
  *
  */
 public class TestSSL {
 
    public static void main(String[] args) {
         // default url:
 
        String urlString = "https://cta03.rchland.ibm.com";
         // if any url specified, use that instead:
 
        if(args.length > 0) {
             urlString = args[0];
         }
         System.out.println("Connecting to " + urlString + "...");
 
        try {
             // convert user string to URL object
 
            //URL url = new URL(urlString);
 
            // connect!
 
            //URLConnection cnx = url.openConnection();
            //HttpsURLConnection cnx = (HttpsURLConnection)url.openConnection();
            HttpsURLConnection cnx = (HttpsURLConnection) new URL(urlString).openConnection();
             cnx.connect();
 
            // read the page returned
 
            InputStream ins = cnx.getInputStream();
             BufferedReader in = new BufferedReader(new InputStreamReader(ins));
             String curline;
             while( (curline = in.readLine()) != null ) {
                 System.out.println(curline);
             }
 
            // close the connection
 
            ins.close();
         }
         catch(Throwable t) {
             t.printStackTrace();
         }
 
    }
 }

