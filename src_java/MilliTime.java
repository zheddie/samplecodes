
public class MilliTime {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		long  time1 = 0L,time2=0L;
		long t = 0L;
		for (long i =0;i<1000*1000;i++){
			time1 = System.currentTimeMillis();
			t = i * 2;
			time2 = System.currentTimeMillis();
			if(time2-time1 < 0){
				System.out.println("ERROR:time2-time1="+(time2-time1)+",t="+t);
				break;
			}
		}
	}

}
