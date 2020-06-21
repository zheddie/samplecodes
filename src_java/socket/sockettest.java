import java.net.Socket;
public class sockettest{
	public static void main(String args[]){
		System.out.println("zg.main().cp0");
		Socket st = null;
		try{
			st = new Socket(args[0],Integer.parseInt(args[1]));
			System.out.println("zg.main().cp1");
		}catch(Exception e){
			System.out.println("zg.main().cp2");
			e.printStackTrace();
			System.out.println("zg.main().cp3");
		}
		System.out.println("zg.main().cp4,st:"+st.toString());
	}
}