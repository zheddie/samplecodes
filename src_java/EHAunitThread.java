class Connectthread implements Runnable
{
public void run()
{
    for(int j=0;j<90;j+=10)
    System.out.println("Connecting..." + j + " Secs");
}
}

class DLoadthread implements Runnable
{
public void run()
{
    for(int d=0;d<60;d+=10)
    System.out.println("Downloading..." + d + " Secs");
}
}
class Runthread implements Runnable
{
public void run()
{
    for(int r=0;r<120;r+=10)
    System.out.println("Running..." + r + " Secs");
}
}
public class EHAunitThread
{
    static class Connectthread ct=new Connectthread();
    static class DLoadthread dt=new DLoadthread();
    static  class Runthread rt=new Runthread();

    static public void main(String arg[])
    {

        //putting threads into ready state.
        System.out.print("Starting threads\n");
        ct.start();
        dt.start();
        rt.start();

        System.out.print("Sleeping 3 seconds\n");
        safeSleep(3000, "Threads first sleep time interrupted\n");
        System.out.print("Suspending threads\n");
        ct.suspend();
        dt.suspend();
        rt.suspend();

        System.out.print("Sleep 5 seconds\n");
        safeSleep(5000, "Threads second sleep time interrupted\n");
        System.out.print("Resume threads\n");
        ct.resume();
        dt.resume();
        rt.resume();

        try
        {
            ct.join();
            dt.join();
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
