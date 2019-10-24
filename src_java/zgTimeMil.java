import java.util.Date;
import java.util.Locale;


public class zgTimeMil extends Thread
{

	public static void main(String[] args)
	{
		zgTimeMil ztt[] = new zgTimeMil[10];
		
		for(int i =0;i<10;i++){
			try{
				ztt[i] = new zgTimeMil();
				ztt[i].start();
			}
			catch(Exception e){
				System.out.println("i="+i);
				e.printStackTrace();
			}
		}
		for(int j =0;j<10;j++){
			try {
				ztt[j].join();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		}	

	}

	@Override
	public void run() {
		// TODO Auto-generated method stub
		
		long iCount = 0;
		long start = 0;
		System.out.println("Thread["+Thread.currentThread().getId()+"]created!");
		while(true)
		{
			try
			{
				start = System.currentTimeMillis();
				// Generate start currentTimeMillis
				try{
					Thread.sleep(100);
				}catch (Exception e){
					
				}
				long newTime = System.currentTimeMillis();
				long diff = newTime - start ;
				
				iCount++;
				if(diff > 1000 || diff < 0 ){
					Date dt = new Date(newTime);
					
					System.out.println("Error!!["+Thread.currentThread().getId()
							+"]Diff="+diff+",Count="+iCount+",newTime="+dt.toString()
							+"oldTime = "+ start
							+"newTime = "+ newTime
							);
				}
				
				
			}
			catch( Exception e )
			{

			}

		}
	}
}

