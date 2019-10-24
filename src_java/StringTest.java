import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.Vector;



public class StringTest implements Runnable {

	/**
	 * @param args
	 */
	public static HashMap<String, String> zgHash = new HashMap<String, String>();
    public static Vector<Thread>        threadList;
    public static long sumTotal = 0;
    public static TreeMap<Long, Long> tm = null;
    public static boolean bFlag = false;
    static Object  sumTotalMonitor = null;
    public static String dmy = "key=value";
    public static String dmyC = null;
    public static String [] sKeys = null;
    public static int hashEntries = 0;
	public static void main(String[] args) throws InterruptedException {
		// TODO Auto-generated method stub
		int iThrdCount = 8;
		//System.out.print("args[1]="+args[1]);
		if(args.length > 0){
			iThrdCount = Integer.valueOf(args[0]).intValue();
		}
		String fileName = "./customerhashmap.txt";
		
		
		if(args.length > 1){
			hashEntries = Integer.valueOf(args[1]).intValue();
		}
		if(args.length > 2){
			int iFlag = Integer.valueOf(args[2]).intValue();
			if(iFlag != 0 ){
				bFlag = true;
			}

		}
		if(hashEntries == 0){
			hashEntries = 76800;
		}
		if(hashEntries< 16000){
			hashEntries = 16000;
		}
		sKeys = new String[hashEntries];
		sumTotalMonitor = new Object();
		tm = new TreeMap<Long, Long>();
		try{
			BufferedReader reader = new BufferedReader(new FileReader(fileName));
			String line = null;
			String [] keyVal = null;
			int index = 0;
			while ((line = reader.readLine()) != null) {
				//System.out.println(line);
			    keyVal = line.split("\\|");
			    //System.out.println(keyVal[0]+"="+keyVal[1]);
			    //zgHash.put(keyVal[0], keyVal[1]);
			    
			    sKeys[index] = keyVal[0]; 
			    index ++;
			    if(index >= hashEntries){
			    	break;
			    	}
			}
		}
		catch(Exception e){
			System.out.println("zg.Exception:"+e.getMessage());
		}
       	
//        Set <String> kSet = zgHash.keySet();
//        Object [] aKSet = kSet.toArray();
//        String sKeyValue = "";
//        
//        for(int j = 0 ;j<aKSet.length;j++){
//        	sKeyValue += ","+aKSet[j].toString()+"|"+zgHash.get(aKSet[j]);
//        }		
//        System.out.println("sKeyValue="+sKeyValue);
		
		threadList = new Vector<Thread>();

		//System.out.println("zgHash.size="+zgHash.size());
		//HashMapTest [] ht=new HashMapTest[128]; 
		for(int i =0;i<iThrdCount;i++){
			StringTest ht = new StringTest();
			Thread aT = new Thread(ht);
			threadList.add(aT);
			aT.start();
		}
		while (!threadList.isEmpty()) {
            try {
                while (threadList.firstElement().isAlive()) {
                	Thread.sleep(500);
                }
                threadList.remove(0);
            }
            catch (NoSuchElementException e) {
                System.out.println("No first element in the thread list: ");
                e.printStackTrace();
            }
        }		
		if(bFlag){
			Set s = tm.entrySet();
			Iterator it = s.iterator();
			while(it.hasNext()){
				Map.Entry me = (Map.Entry)it.next();
				System.out.println(me.getKey()+"="+me.getValue());
			}
		}
		System.out.println("zgHash.size="+zgHash.size()+",ThreadsNum="+iThrdCount+",SumTotal="+sumTotal+",avg="+sumTotal/iThrdCount);
//		System.out.println("Main program exit.");
//        long timeMill = System.currentTimeMillis();
//		zgHash.put("zgkey", "zgValue");
//		long diff = System.currentTimeMillis() - timeMill;
//		System.out.println("zgHash.size="+zgHash.size());
//        System.out.println("putTime="+diff);
        
        
	}

	@Override
	public void run() {
		long tID = Thread.currentThread().getId();
		// TODO Auto-generated method stub
		String Key1 = "360291268724523011";
		String Key2 = "360291268724523011";
		//String val = null;
		int s = 0;
		long timeMill  = 0;
		long diff = 0;
		//System.out.println("In child!");
		
		long sum = 0;
		for(s = 0;s<2000*10000;s++){
//			int l = s% hashEntries ;
//			String Key1 = sKeys[l];
//			String Key2 = sKeys[hashEntries -1 -l];
			timeMill = System.currentTimeMillis();

			//val = zgHash.get(searchKey);
			//dmyC = dmy+s;
			boolean b =(Key1 == Key2)||Key1.equals(Key2);
			diff = System.currentTimeMillis() - timeMill;

			sum += diff;
			if(diff > 100){
				System.out.println("zg.StringTest.tID="+tID+",equal?="+b+",timeUsed="+diff+",count="+s);
			}
		}
		synchronized(sumTotalMonitor){
			sumTotal+=sum;
			if(bFlag){
				tm.put(tID, sum);
			}
		}
//		System.out.println("tID="+tID+",sum="+sum);		
	}

}
