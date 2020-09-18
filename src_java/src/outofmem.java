
import java.io.PrintStream;
import java.lang.Integer;
import java.lang.StringBuffer;
import java.lang.OutOfMemoryError;


public class outofmem
{
 	public static void main(String args[])
	{
		System.exit(run(args, System.out) + 95/*STATUS_TEMP*/); 
	} 
 
	public static int run(String args[], PrintStream out)
	{
        int i = 0;
        // total memory allocation is ~20 000 TeraBytes
        try {
            int size = 10240000;
            StringBuffer[] buf = new StringBuffer[size];

            for (i = 0; i < size; ++i) {
                if(i % 1000 == 0){
                    out.println("i="+i+",Integer.MAX_VALUE="+Integer.MAX_VALUE);
                }
                buf[i] = new StringBuffer(Integer.MAX_VALUE);
            }
        } catch (OutOfMemoryError e) {
            out.println("OutofMemoryError hit! i="+i);
            return 0;
        }
        return 2;	    
	}
}