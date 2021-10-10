import java.util.zip.*;
import java.util.Enumeration;
import java.util.*;
import java.io.*;
import java.nio.charset.Charset;

public class solution {
    public static void main(String args[])
    {
        try {
            // Create a Zip File
            ZipFile zip_file = new ZipFile("./file.zip",Charset.forName("utf8"));
  
            // get the Zip Entry using
            // the getEntry() function
            ZipEntry entry = zip_file.getEntry("file1.cpp");
  
            // get the Input Stream
            // using the getInputStream()
            // function
            InputStream input = zip_file.getInputStream(entry);
  
            // Create a scanner object
            Scanner sc = new Scanner(input);
  
            System.out.println("Contents:");
  
            // Display the contents Zip Entry
            while (sc.hasNext()) {
                System.out.println(sc.nextLine());
            }
  
            // Close the scanner
            sc.close();
        }
        catch (Exception e) {
            System.out.println(e.getMessage());
        }
    }
}