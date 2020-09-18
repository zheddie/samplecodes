import java.io.*;
import java.nio.file.Paths;
public class RuntimeExecTest{
    public static void main(String[] args) throws IOException{
        // String JAVA_LAUNCHER = Paths.get(System.getProperty("java.home")).resolve("bin").resolve("java").toString();
        // System.out.println("cmd is:"+ JAVA_LAUNCHER);
        System.out.println("args:");
        for(String x:args){
        	System.out.print(" "+x);
        }
        System.out.println();
        Process process = Runtime.getRuntime().exec(args);
        System.out.println("pid:"+process.pid());
		BufferedReader br = new BufferedReader(new InputStreamReader(process.getErrorStream()));
        String line;
        while ((line = br.readLine()) != null) {
            System.out.println("zg.line:"+line);
            // do nothing
        }
        System.out.println("zg.EXIT:");      
    }
}