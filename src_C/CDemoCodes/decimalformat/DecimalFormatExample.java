import java.text.DecimalFormat;

public class DecimalFormatExample {   

	public static void main(String args[])  {

		//formatting numbers upto 2 decimal places in Java
		DecimalFormat df = new DecimalFormat("#,###,##0.00");
		System.out.println(df.format(364565.14));
		System.out.println(df.format(364565.1454));
		Double d = new Double(-0.0);
		//formatting numbers upto 3 decimal places in Java
		df = new DecimalFormat("#,###,##0.000");
		System.out.println(df.format(364565.14));
		System.out.println(df.format(364565.1454));
		System.out.println(df.format(d));
	}
}
