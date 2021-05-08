import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class ReadFile {

	public static void main(String[] args) {
		System.out.println("file.encoding="+System.getProperty("file.encoding"));
		BufferedReader reader;
		try {
			reader = new BufferedReader(new FileReader(
					args[0]));
			String line = reader.readLine();
			while (line != null) {
				System.out.println(line);
				// read next line
				line = reader.readLine();
			}
			reader.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}

