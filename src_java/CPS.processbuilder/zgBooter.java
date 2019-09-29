import java.io.File;
import java.io.IOException;
import java.lang.ProcessBuilder.Redirect;
import java.lang.management.ManagementFactory;
import java.lang.management.RuntimeMXBean;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.io.InputStream;

public class zgBooter {

	public static void main(String[] args) {
		System.out.println("zgBooter.cp0!!ＡＢＣＤ");
                System.out.println("p.JAVA_HOME="+System.getenv("JAVA_HOME")); 
                System.out.println("p.QIBM_PASE_DESCRIPTOR_STDIO="+System.getenv("QIBM_PASE_DESCRIPTOR_STDIO")); 
                System.out.println("p.QIBM_JAVA_CHILD_STDIO_CONVERT="+System.getenv("QIBM_JAVA_CHILD_STDIO_CONVERT")); 
                System.out.println("p.PASE_QIBM_JAVA_CHILD_STDIO_CONVERT="+System.getenv("PASE_QIBM_JAVA_CHILD_STDIO_CONVERT")); 
                System.out.println("p.QIBM_JAVA_STDIO_CONVERT="+System.getenv("QIBM_JAVA_STDIO_CONVERT")); 
		String javaPath = Paths.get(System.getProperty("java.home"), "bin", "java").toString();

		RuntimeMXBean runtime = ManagementFactory.getRuntimeMXBean();
		List<String> javaOptionParams = runtime.getInputArguments();
		List<String> commandLine = new ArrayList<>();
		commandLine.add(javaPath);
		commandLine.addAll(javaOptionParams);

		commandLine.add("-cp");
		//commandLine.add("/home/pmr42254");
		//commandLine.add("/Users/zhanggan/mycode/zgJava/CPS.processbuilder");
		commandLine.add("/qopensys/home/zhanggan");

		commandLine.add("SampleApplication");
		commandLine.add(args[0].trim());

		ProcessBuilder pb = new ProcessBuilder();
		pb.command(commandLine);
		pb.directory(new File(System.getProperty("java.io.tmpdir")));
		System.out.println("zgBooter.cp1!!ＡＢＣＤ");
//pb.inheritIO();
//pb.redirectError(Redirect.INHERIT);
//pb.redirectInput(Redirect.INHERIT);
  pb.redirectOutput(Redirect.INHERIT);
		System.out.println("zgBooter.cp2!!ＡＢＣＤ");
                System.out.println("p.JAVA_HOME="+System.getenv("JAVA_HOME")); 
                System.out.println("p.QIBM_PASE_DESCRIPTOR_STDIO="+System.getenv("QIBM_PASE_DESCRIPTOR_STDIO")); 
                System.out.println("p.QIBM_JAVA_CHILD_STDIO_CONVERT="+System.getenv("QIBM_JAVA_CHILD_STDIO_CONVERT")); 
                System.out.println("p.PASE_QIBM_JAVA_CHILD_STDIO_CONVERT="+System.getenv("PASE_QIBM_JAVA_CHILD_STDIO_CONVERT")); 
                System.out.println("p.QIBM_JAVA_STDIO_CONVERT="+System.getenv("QIBM_JAVA_STDIO_CONVERT")); 
		// StartProcess
		try {
			int i;
			char c;
			Process pr = pb.start();
			InputStream is = pr.getInputStream();
			if(is != null){
			while((i=is.read())!=-1){
				c = (char ) i;
				System.out.print(c);
			}
			System.out.println();
			}
			int eixtCode = pr.waitFor();
			System.out.println("SubProcess exitCode => " + eixtCode);
		} catch(IOException | InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println("zgBooter.cp3!!ＡＢＣＤ");
                System.out.println("p.JAVA_HOME="+System.getenv("JAVA_HOME")); 
                System.out.println("p.QIBM_PASE_DESCRIPTOR_STDIO="+System.getenv("QIBM_PASE_DESCRIPTOR_STDIO")); 
                System.out.println("p.QIBM_JAVA_CHILD_STDIO_CONVERT="+System.getenv("QIBM_JAVA_CHILD_STDIO_CONVERT")); 
                System.out.println("p.PASE_QIBM_JAVA_CHILD_STDIO_CONVERT="+System.getenv("PASE_QIBM_JAVA_CHILD_STDIO_CONVERT")); 
                System.out.println("p.QIBM_JAVA_STDIO_CONVERT="+System.getenv("QIBM_JAVA_STDIO_CONVERT")); 
	}
}
