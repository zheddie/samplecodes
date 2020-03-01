import java.io.*;

public class CreateTemp {

    /**
     * @param args
     */
    public static void main(String[] args) {
        // TODO Auto-generated method stub
        try {

            // Create temporary file.
            //File tempfile = File.createTempFile("name", ".ext");
            File dir = new File("/home/zhanggan/temp/noexist");
            File tempfile = File.createTempFile("name", ".ext",dir);
        
            // Delete temp file when program exits.
            tempfile.deleteOnExit();
            System.out.println("path="+tempfile.getPath());
            // Write to temp file
            BufferedWriter out = new BufferedWriter(new FileWriter(tempfile));
            out.write("aString");
            out.close();
            

        } catch (IOException e) {
           
            System.out.println("Exception:"+e.getMessage());
            e.printStackTrace();
        }
    }

}
