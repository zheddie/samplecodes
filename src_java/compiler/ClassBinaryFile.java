package compiler;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.URI;
import java.net.URISyntaxException;

import javax.tools.SimpleJavaFileObject;

public class ClassBinaryFile extends SimpleJavaFileObject {

	ByteArrayOutputStream buf = new ByteArrayOutputStream();
	
	public ClassBinaryFile(String className) throws URISyntaxException {
		super(new URI(className + ".class"), Kind.SOURCE);
	}

	public OutputStream openOutputStream() throws IOException {
		return buf;
	}
	
	public byte[] getBuffer() {
		return buf.toByteArray();
	}
}