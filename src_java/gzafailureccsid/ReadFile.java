import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class ReadFile {
    private static final String    HEXES    = "0123456789ABCDEF";

    static String getHex(byte[] raw) {
        final StringBuilder hex = new StringBuilder(2 * raw.length);
        for (final byte b : raw) {
            hex.append(HEXES.charAt((b & 0xF0) >> 4)).append(HEXES.charAt((b & 0x0F)));
        }
        return hex.toString();
    }
    static public byte[] string2Bytes(String str)
    {
        char[] chars = str.toCharArray();
        byte[] bytes = new byte[chars.length];
        for (int i = 0; i < chars.length; i++)
        {
            bytes[i ] = (byte) chars[i];
        }
        return bytes;
    }
	public static void main(String[] args) {
		System.out.println("file.encoding="+System.getProperty("file.encoding"));
		BufferedReader reader;
		try {
			reader = new BufferedReader(new FileReader(
					args[0]));
			String line = reader.readLine();
			while (line != null) {
				System.out.println(getHex(string2Bytes(line)));
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

