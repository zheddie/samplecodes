import java.io.File;
import java.io.IOException;
import java.lang.ProcessBuilder.Redirect;
import java.lang.management.ManagementFactory;
import java.lang.management.RuntimeMXBean;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;

public class Booter5 {

	public static void main(String[] args) {
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
//pb.inheritIO();
//pb.redirectError(Redirect.INHERIT);
//pb.redirectInput(Redirect.INHERIT);
  pb.redirectOutput(Redirect.INHERIT);

		// StartProcess
		try {
			Process pr = pb.start();
			int eixtCode = pr.waitFor();
			System.out.println("SubProcess exitCode => " + eixtCode);
		} catch(IOException | InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
