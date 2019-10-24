package compiler;

import java.util.Map;

public class MyClassLoader extends ClassLoader {

	private Map<String, ClassBinaryFile> binaries;
	
	MyClassLoader(Map<String, ClassBinaryFile> binaries) {
		this.binaries = binaries;
	}
	
	public Class<?> findClass(String name) {
        byte[] b = binaries.get(name).getBuffer();
        return defineClass(name, b, 0, b.length);
    }
}
