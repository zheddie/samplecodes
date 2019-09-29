import java.nio.charset.Charset;
class WangPan{
	public static void main(String[] args) {
		System.out.println(System.getProperty("file.encoding"));
		System.out.println(System.getProperty("default.client.encoding"));
		System.out.println(Charset.defaultCharset());
		String str1 = "A m\u0171velet nem \u00e9rv\u00e9nyes: {0}";
		String str2 = "A(z) {0} param\u00e9ter megad\u00e1sa k\u00f6telez\u0151";
		System.out.println(str1);
		System.out.println(str2);
		System.out.println("\u0171");
		System.out.println("\u0151");
		System.out.println("ű and ő");

		System.out.println("End");
	}



}
