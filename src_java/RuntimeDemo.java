/*
import java.io.*;
public class RuntimeDemo {

	public static void main(String[] args) {
		try {
			String line;
			System.out.println("Exeecuting unname");
			Process process = Runtime.getRuntime().exec("uname");
			BufferedReader input = new BufferedReader(new InputStreamReader(process.getInputStream()));
            while((line = input.readLine()) != null)
                System.out.println(line);
            input.close();
            System.out.println("uname has been executed");
		}catch (Exception e) {
			e.printStackTrace();
		}
		// TODO Auto-generated method stub

	}

}
*/
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.concurrent.TimeUnit;

/**
 * Created by benjamin on 12/23/15.
 */
public class RuntimeDemo {

    public static void main(String[] args) {
        /*System.out.println("开始休眠\n");
        try {
            TimeUnit.HOURS.sleep(0);
            TimeUnit.MINUTES.sleep(1);
            TimeUnit.SECONDS.sleep(16);
            TimeUnit.MILLISECONDS.sleep(132);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println("休眠结束\n");
        */
        String s = null;
        try {
            Process p = Runtime.getRuntime().exec("ps -ef");
            BufferedReader stdInput = new BufferedReader(new InputStreamReader(p.getInputStream()));
            BufferedReader stdError = new BufferedReader(new InputStreamReader(p.getErrorStream()));
            //从命令行打印出输出结果
            System.out.println("标准输出命令\n");
            while ((s = stdInput.readLine()) != null) {
                System.out.println(s);
            }

            System.out.println("标准错误的输出命令(如果存在):\n");
            while ((s = stdError.readLine()) != null) {
                System.out.println(s);
            }
            System.exit(0);
        } catch (IOException e) {
            System.out.println("异常发生: ");
            e.printStackTrace();
            System.exit(1);
        }
    }
}