import com.ibm.lang.management.*;

public class MbeanTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		ExtendedOperatingSystem exb = ExtendedOperatingSystem.getInstance();
		//Init call to this method to store the initial values.
		exb.getSystemCpuLoad();
		exb.getProcessCpuLoad();
		
		for(long i =0;i<1000*1000;i++){
			if(i%10000 == 0){
				System.out.println("i="+i);
			}
		}
		double dsysload = exb.getSystemCpuLoad();
		double dprcload = exb.getProcessCpuLoad();
		
		System.out.println("SystemLoad="+dsysload);
		System.out.println("ProcessLoad="+dprcload);
		int cpuCount = exb.getProcessingCapacity();
		System.out.println("cpuCount="+cpuCount);
	}


}
