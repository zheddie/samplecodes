/*copyright test*/

import java.lang.Double;
import java.text.DecimalFormat;

/*class description*/
public class zHello {

 public static void main(String[] args){
  System.out.println("Start");
   System.out.println("java version:"+System.getProperty("java.version"));
   System.out.println("java home:"+System.getProperty("java.home"));
   DecimalFormat df=new DecimalFormat();
	 Double d=new Double(-0.0);
	 double number=d.doubleValue();
	 System.out.println(1/d.doubleValue());
	 boolean isNegative = ((number < 0.0) || (number == 0.0 && 1/number < 0.0)) ^ (1 < 0);
	 System.out.println(isNegative);  
	 System.out.println(df.format(d));
	 System.out.println("end!");
 }
}
