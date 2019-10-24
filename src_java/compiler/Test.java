package compiler;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.URISyntaxException;
import java.util.Random;

public class Test {
	
	private static Random rand = new Random();
	
	public static void main(String[] args) throws URISyntaxException,
			IOException, ClassNotFoundException, IllegalAccessException,
			InstantiationException {
		
		long t = System.currentTimeMillis();
		for (int i = 0;; i++) {
			String className = randomName();
			String content = composeAProgram(className);
			
			TestJavaCompiler.testCompileAndRun(content);
		
			if (i % 1000 == 0) {
				System.out.println("Count=" + i + " - " + className 
						+ ". Elapsed=" + ((System.currentTimeMillis() - t) / 1000)
						+ ". Runtime.totalMemory()=" + Runtime.getRuntime().totalMemory());
			}
		}
		
		/*
		System.out.println("GC...");
		System.gc();
		System.out.println("Done. Sleeping.");
		try {
			while (true)
				Thread.sleep(1000);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		*/
	}
	
	private static String composeAProgram(String className) {
		StringWriter sw = new StringWriter();
		PrintWriter pw = new PrintWriter(sw);

		pw.println("import compiler.SomeInterface;");
		
		pw.println("public class " + className + " implements SomeInterface {");
		pw.println("	public static final String s = \"verylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstringverylongstring\";");
		pw.println("	public static class StaticClass {}");
		pw.println("	public class InnerClass {}");
		pw.println("	public int f(int n) {");
		pw.println("		class MethodClass {}");
		pw.println("		//System.out.println(new StaticClass().hashCode());");
		pw.println("		//System.out.println(new InnerClass().hashCode());");
		pw.println("		//System.out.println(new MethodClass().hashCode());");
		pw.println("		return n + 1;");
		pw.println("	}");
		pw.println("}");
		pw.close();
		return sw.toString();
	}
	
	private static String randomName() {
		String s = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
		int len = rand.nextInt(20) + 10;
		StringBuilder sb = new StringBuilder(len);
		for (int i = 0; i < len; i++) {
			int n = rand.nextInt(s.length());
			sb.append(s.charAt(n));
		}
		return sb.toString();
	}
}
