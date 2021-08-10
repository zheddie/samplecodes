import java.util.Date;

public class zgSleep{
	public static void main(String[] args) {

		if(args.length <1){
			System.out.println("Usage:\n\t zgSleep <count of seconds>\n");
			return;
		}
		int count = Integer.parseInt(args[0]);
		System.out.println("Start sleeping "+count+" seconds..."+new Date());
		try{
			Thread.sleep(1000*count);
		}catch(Exception e)
		{
			System.out.println("Exception occurred");
		}
		System.out.println("End sleep for "+count+" seconds ..."+new Date());
	}

}
