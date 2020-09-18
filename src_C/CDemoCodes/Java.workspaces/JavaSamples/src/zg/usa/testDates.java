package zg.usa;
import java.text.DateFormat;
import java.util.Calendar;
import java.util.Locale;


public class testDates {
  public static void main(String[] args){
	  Calendar cal = Calendar.getInstance();
	  cal.set(1997, 9, 25);
	  Locale [] locals = new Locale[3];
	  locals[0]=new Locale("en","US");
	  locals[1]=new Locale("zh","CN");
	  locals[2]=new Locale("ja","JP");
	  for(int i =0;i<3;i++){
		  DateFormat df = DateFormat.getDateInstance(DateFormat.MEDIUM, locals[i]);
		  String testDate = df.format(cal.getTime());
		  System.out.println(locals[i].getDisplayName()+"="+testDate);
	  }
	}
}
