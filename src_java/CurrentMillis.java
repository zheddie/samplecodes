import java.util.Date;
import java.util.Locale;

public class CurrentMillis 
{
	public static void main(String[] args)
	{
		int toSleep = 5000;
		int iTimes = 3;
		int iMaxCount = 10;
		if(args.length >= 1){
			toSleep = Integer.valueOf(args[0]).intValue();
		}
		if(args.length >= 2){
			iTimes = Integer.valueOf(args[1]).intValue();
		}
		if(args.length >= 3){
			iMaxCount = Integer.valueOf(args[2]).intValue();
		}		
		int count = 0;
		int errCount = 0;
		long errDiff = 0;
		for( ;errCount <= iMaxCount ; )
		{
			try
			{

				// Generate start currentTimeMillis
				long start = System.currentTimeMillis();
				//long start = System.nanoTime();
				
//				System.out.println("Start time in millis: " + start);

				// Sleep
				// If ISeries/AIX NTC Client adjusts time during the sleep then the nanoTime leaps forwards or backwards !
				// May take a few adjustments for this to happen.
				Thread.sleep( toSleep );		
				count ++;
				// Generate current millis time
				long now = System.currentTimeMillis();
				//long now = System.nanoTime();
				Date dt = new Date(now);
				long diff = now - start;
				

				if(diff < 0 ||diff > iTimes* toSleep ){
					errCount ++;
					errDiff =  diff;
					System.out.println( "ERROR.diff="+diff +",start="+start+",now="+now+",Date.now()="+ dt.toString()
							+",count="+count
							+",errCount="+errCount
							);
				}else{
					if ((count*toSleep) % 5000 == 0 ) {
						System.out.println( "diff="+diff
								+",count="+count
								+",errCount="+errCount
								+",errDiff="+errDiff
								);
					}				
				}
				// If start time greater than current then this is an error so exit
//				if( diff > 50 || diff <5)
//				{
//					count ++;
//					Date dt1 = new Date(now/1000/1000);
//					System.out.println( "diff="+diff +",start="+start+",now="+now+",Date.now()="+ dt1.toString());
//				}
			}
			catch( InterruptedException e )
			{

			}

		}

	}
}
