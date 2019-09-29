
class Runthread extends Thread 
{
    public void run()
    {
        for(int r=0;r<12;r+=1){
        try{
            Thread.sleep(1000);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        System.out.println("Running..." + r + " Secs");
        }
    }
}
public class threadSuspend
{
    static  Runthread rt=new Runthread();

    static public void main(String arg[])
    {

        //putting threads into ready state.
        System.out.print("Starting threads\n");
        rt.start();

        System.out.print("Sleeping 3 seconds before suspend\n");
        try{
            Thread.sleep(3000);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        // System.out.print("Suspending threads\n");

        rt.suspend();
        System.out.print("Sleeping 3 seconds before resume\n");
        try{
            Thread.sleep(3000);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }

        rt.resume();

        try
        {
            rt.join();
        }
        catch (InterruptedException e)
        {
            System.out.print("Join interrupted");
        }

        System.out.print("Testcase Completed");
        System.exit(0);
    }
}
