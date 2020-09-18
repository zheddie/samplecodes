package java.lang;
/**
* This is comment for ZGDEBUG
*/
public class ZDEBUG{
	/**
	* This is comment for ztrace.
	* @param ln The line for print.
	*/
	public static void ztrace(String ln){
		ln="zg.["+Thread.currentThread().getId()+"]"+ln;
		System.out.println(ln);
	}
}