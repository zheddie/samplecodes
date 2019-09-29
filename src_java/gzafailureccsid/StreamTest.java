import java.lang.*;
import java.io.*;

public class StreamTest {
	public static void main(String args[]) 
    {
	try{
			SerializableTestClass myClass = new SerializableTestClass();
			System.out.println(myClass);
			FileOutputStream fos = new FileOutputStream("SerializableTestFile.old");
			ObjectOutputStream objOut = new ObjectOutputStream(fos);
			objOut.writeObject(myClass);
			objOut.flush();
			objOut.close();

    	    // Damage the file that was written out.
    	    FileReader oldFile = null;
    	    BufferedReader ofBr = null;	
    	    FileWriter newFile = null;
    	    BufferedWriter nfBw = null;

    	    oldFile = new FileReader("SerializableTestFile.old");
    	    ofBr = new BufferedReader(oldFile);

    	    newFile = new FileWriter("SerializableTestFile.new");
    	    nfBw = new BufferedWriter(newFile);

			String in = "";
    	    String s = "11111111111111111111111111111111111";
    	    while ((in = ofBr.readLine()) != null) {
    	    	nfBw.write(in);
    	    }
    	    nfBw.write(s);
    	    nfBw.newLine();
    	    nfBw.flush();
    	    nfBw.close();
			
			// Attempt to read the class.
    	    try
    	    {
        		FileInputStream fis = new FileInputStream("SerializableTestFile.new");
        		ObjectInputStream objIn = new ObjectInputStream(fis);
        		SerializableTestClass myNewClass = (SerializableTestClass)objIn.readObject();
				System.out.println(myNewClass);
        		objIn.close();
        		// This should never be reached.
				System.out.println("ERROR: Exception not thrown.");
				return;
            }
			catch (UTFDataFormatException ex)
        	    {
        		System.out.println(ex);
				System.out.println("passed");
        		return;
        	    }        	   
			   catch (Exception ex)
        	    {
        		System.out.println("ERROR: Wrong kind of exception was thrown: ");
				ex.printStackTrace(System.out);
				return;
				}
    	}
		catch (IOException ex)
    	{
    	    ex.printStackTrace(System.out);
			System.out.println("ERROR: Exception during testcase execution: ");
    	    return;
    	}

	}
}