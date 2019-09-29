import java.util.*;
import java.text.*;
class getEraDate{
	public static void main(String s[]){
		SimpleDateFormat sdf =new SimpleDateFormat("GGGGyyyy/M/d", Locale.forLanguageTag("ja-JP-u-ca-japanese"));
		System.out.println(sdf.format(new Calendar.Builder().setDate(2019, Calendar.MAY, 1).build().getTime()));
		System.out.println(sdf.format(new Calendar.Builder().setDate(2019, Calendar.APRIL, 1).build().getTime()));
	}
}
