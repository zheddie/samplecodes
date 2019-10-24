package compiler;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;

import javax.tools.SimpleJavaFileObject;

public class SourceFile extends SimpleJavaFileObject {

	private String content;
	
	public SourceFile(String className, String content) throws URISyntaxException {
		super(new URI(className + ".java"), Kind.SOURCE);
		this.content = content;
	}

	@Override
	public CharSequence getCharContent(boolean ignoreEncodingErrors)
			throws IOException {
		return content;
	}
}