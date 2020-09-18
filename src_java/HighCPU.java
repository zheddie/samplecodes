
public class HighCPU {

	/**
	 * @param args
	 */
	public static  int i =0;
	public static void main(String[] args) {
		
		// TODO Auto-generated method stub
		while(true){
			printAll();
		}
	}
	public static void printAll(){
		i = i+1;
		if(i%1000000==0){
			System.out.println("i="+i+"\n");
		}
	}
}
