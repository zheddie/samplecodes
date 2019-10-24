import java.lang.*;
import com.ibm.as400.access.JobCCSID;                       
import com.ibm.as400.access.AS400;   
public class getJobCCSID{
	public static void main(String[] args) throws Exception {
		System.out.println("bootClassPath="+System.getProperty(args[0]));
		//String jobCCSID = JobCCSIDNative.retrieveCcsid();
		int jobCCSID = new JobCCSID(new AS400()).retrieveCcsid();
		System.out.println("jobCCSID="+jobCCSID);
	}

}
