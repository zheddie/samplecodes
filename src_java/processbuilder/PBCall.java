import java.lang.ProcessBuilder;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.nio.file.Paths;
import java.io.File;

public class PBCall{
	public static void main(String [] args){
		String JAVA_LAUNCHER = Paths.get(System.getProperty("java.home")).resolve("bin").resolve("java").toString();
        System.out.println("cmd is:"+ JAVA_LAUNCHER);

        ProcessBuilder pb;
        if(args.length>0){
        	pb = new ProcessBuilder(JAVA_LAUNCHER,"-cp",".",args[0]);
        }else if(args.length>1){
        	pb = new ProcessBuilder(JAVA_LAUNCHER,"-cp",".",args[0],args[1]);
        }else{
        	pb = new ProcessBuilder(JAVA_LAUNCHER);
        }
        //pb.directory(new File("thisWorkingDirectoryDoesNotExists_111222333"));
        try{
        	Process process = pb.start();
			BufferedReader brerror = new BufferedReader(new InputStreamReader(process.getErrorStream()));
			BufferedReader broutput = new BufferedReader(new InputStreamReader(process.getInputStream()));
	        String line;
	        while ((line = brerror.readLine()) != null) {
	            System.out.println("zg.stedrr.line:"+line);
	            // do nothing
	        }
	        while ((line = broutput.readLine()) != null) {
	            System.out.println("zg.stdout.line:"+line);
	            // do nothing
	        }
        }catch ( Exception e){
        	e.printStackTrace();
        }
        pb=new ProcessBuilder(JAVA_LAUNCHER);
        //pb.directory(new File("thisWorkingDirectoryDoesNotExists_111222333"));
        try{
        	Process process = pb.start();
			BufferedReader brerror = new BufferedReader(new InputStreamReader(process.getErrorStream()));
			BufferedReader broutput = new BufferedReader(new InputStreamReader(process.getInputStream()));
	        String line;
	        while ((line = brerror.readLine()) != null) {
	            System.out.println("zg.stedrr.line:"+line);
	            // do nothing
	        }
	        while ((line = broutput.readLine()) != null) {
	            System.out.println("zg.stdout.line:"+line);
	            // do nothing
	        }
        }catch ( Exception e){
        	e.printStackTrace();
        }
        System.out.println("zg.EXIT:");          
	}
}