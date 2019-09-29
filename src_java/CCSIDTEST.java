import java.nio.charset.Charset;
class CCSIDTEST{
	public static void main(String[] args) {
		System.out.println(System.getProperty("file.encoding"));
		System.out.println(System.getProperty("default.client.encoding"));
		System.out.println(Charset.defaultCharset());
		System.out.println("End");
	}
}
