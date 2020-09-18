/**
 * 
 */
package zg.usa;

/**
 * @author zhanggan
 *
 */
public class GetMyProp {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String mys="12张345";
		forString(mys);
		Thread.dumpStack();
		String s = System.getProperty("myprop");
		forString(s);
	}
	public static void forString(String s){
		if(s != null){
			System.out.println("*"+s+"*");
			byte sb[] = s.getBytes();
			int sbl = sb.length;
			System.out.println("length="+sbl);
			for(int i = 0;i<sbl;i++){
				System.out.print(Integer.toHexString(sb[i])+",");
			}
			System.out.println();
		}else{
			System.out.println("Please give me a string!");
		}		
	}

}
