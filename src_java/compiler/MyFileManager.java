package compiler;

import java.io.IOException;
import java.net.URISyntaxException;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

import javax.tools.FileObject;
import javax.tools.JavaFileManager;
import javax.tools.JavaFileObject;
import javax.tools.JavaFileObject.Kind;

public class MyFileManager implements JavaFileManager {

	private Map<String, ClassBinaryFile> compiledClassBinaries;
	private JavaFileManager fileManager;
	
	MyFileManager(JavaFileManager fileManager, Map<String, ClassBinaryFile> binaries) {
		this.fileManager = fileManager;
		this.compiledClassBinaries = binaries;
	}

	public void close() throws IOException {
		//out.println("close");
		fileManager.close();
	}

	public void flush() throws IOException {
		//out.println("flush");
		fileManager.flush();
	}

	public ClassLoader getClassLoader(Location location) {
		//out.println("getClassLoader");
		return fileManager.getClassLoader(location);
	}

	public FileObject getFileForInput(Location location, String packageName,
			String relativeName) throws IOException {
		throw new UnsupportedOperationException("getFileForInput");
		//out.println("getFileForInput");
		//return fileManager.getFileForInput(location, packageName, relativeName);
	}

	public FileObject getFileForOutput(Location location, String packageName,
			String relativeName, FileObject sibling) throws IOException {
		throw new UnsupportedOperationException("getFileForOutput");
		//out.println("getFileForOutput");
		//return fileManager.getFileForOutput(location, packageName, relativeName, sibling);
	}

	public JavaFileObject getJavaFileForInput(Location location,
			String className, Kind kind) throws IOException {
		throw new UnsupportedOperationException("getJavaFileForOutput");
		//out.println("getJavaFileForInput: " + className);
		//return fileManager.getJavaFileForInput(location, className, kind);
	}

	public JavaFileObject getJavaFileForOutput(Location location,
			String className, Kind kind, FileObject sibling) throws IOException {
		//out.println("getJavaFileForOutput: " + className);
		
		ClassBinaryFile file = null;
		try {
			file = new ClassBinaryFile(className);
			compiledClassBinaries.put(className, file);
		} catch (URISyntaxException e) {
			e.printStackTrace();
		}

		return file;
	}

	public boolean handleOption(String current, Iterator<String> remaining) {
		throw new UnsupportedOperationException("handleOption");
		//out.println("handleOption");
		//return fileManager.handleOption(current, remaining);
	}

	public boolean hasLocation(Location location) {
		//out.println("hasLocation: " + location.getName());
		return fileManager.hasLocation(location);
	}

	public String inferBinaryName(Location location, JavaFileObject file) {
		//out.println("inferBinaryName: " + file.getName());
		return fileManager.inferBinaryName(location, file);
	}

	public boolean isSameFile(FileObject a, FileObject b) {
		throw new UnsupportedOperationException("isSameFile");
		//out.println("isSameFile");
		//return fileManager.isSameFile(b, b);
	}

	public Iterable<JavaFileObject> list(Location location, String packageName,
			Set<Kind> kinds, boolean recurse) throws IOException {
		//out.println("list: " + location.getName());
		return fileManager.list(location, packageName, kinds, recurse);
	}

	public int isSupportedOption(String option) {
		throw new UnsupportedOperationException("list");
		//out.println("isSupportedOption");
		//return fileManager.isSupportedOption(option);
	}
}
