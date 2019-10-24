package compiler;

import static java.lang.System.out;

import java.net.URISyntaxException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.tools.JavaCompiler;
import javax.tools.JavaFileManager;
import javax.tools.JavaFileObject;
import javax.tools.StandardJavaFileManager;
import javax.tools.ToolProvider;

public final class TestJavaCompiler {
	
	private static final Pattern classNamePattern = Pattern.compile(".*public\\s*class\\s*(.*)\\s*implements\\s*SomeInterface\\s*.*");
	
	private static boolean compile(JavaFileObject source, Map<String, ClassBinaryFile> binaries) {
		
		final JavaCompiler compiler = ToolProvider.getSystemJavaCompiler();

		StandardJavaFileManager stdFileManager = compiler.getStandardFileManager(null, null, null);
		JavaFileManager fileManager = new MyFileManager(stdFileManager, binaries);

		final JavaCompiler.CompilationTask task = compiler.getTask(
		/* default System.err */null,
		fileManager,
		/* standard DiagnosticListener */null,
		/* no options */null,
		/* no annotation classes */null,
		Arrays.asList(source));
		return task.call();
	}
	
	public static void testCompileAndRun(String content) throws URISyntaxException {
		
		Map<String, ClassBinaryFile> binaries = new HashMap<String, ClassBinaryFile>();
		
		String className = identifyClassName(content);
		
		JavaFileObject source = new SourceFile(className, content);
				
		boolean status = compile(source, binaries);
		if (!status)
			out.println("Fail compilation!");

		ClassLoader cl = new MyClassLoader(binaries);
		
		try {
			Class<?> c = cl.loadClass(className);
			SomeInterface i = (SomeInterface) c.newInstance();
			if (i.f(3) != 4) {
				System.out.println("Failed!");
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	private static String identifyClassName(String content) {

		content = content.replace('\r', '\n');
		String[] tmp = content.split("\n", 1000);
		for (String s : tmp) {
			Matcher m = classNamePattern.matcher(s);
			if(m.matches()){
				return m.group(1).trim();
			}
		}
		return null;
	}
}

