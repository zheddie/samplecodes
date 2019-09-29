import java.io.OutputStream;
import java.io.FileOutputStream;
public class SampleApplication {
	public static void main(String[] args) {
		//System.setenv("QIBM_JAVA_STDIO_CONVERT","N");
			
		System.out.println("c.JAVA_HOME="+System.getenv("JAVA_HOME")); 
		System.out.println("c.QIBM_PASE_DESCRIPTOR_STDIO="+System.getenv("QIBM_PASE_DESCRIPTOR_STDIO")); 
		System.out.println("c.QIBM_JAVA_CHILD_STDIO_CONVERT="+System.getenv("QIBM_JAVA_CHILD_STDIO_CONVERT")); 
		System.out.println("c.PASE_QIBM_JAVA_CHILD_STDIO_CONVERT="+System.getenv("PASE_QIBM_JAVA_CHILD_STDIO_CONVERT")); 
		System.out.println("c.QIBM_JAVA_STDIO_CONVERT="+System.getenv("QIBM_JAVA_STDIO_CONVERT")); 
		
		System.out.println("Hello, world!!ＡＢＣＤ");
		System.out.println(args[0]);
		System.exit(0);
	}
}
