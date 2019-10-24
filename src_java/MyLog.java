
import java.util.logging.Level;
import java.util.logging.Logger;

public class MyLog {

	/**
	 * @param args
	 */
	  //private static 

	  public static void main(String[] args) {
		  Logger LOGGER = Logger.getLogger("MyLog");
		  System.out.println("Before log,MyLog.level="+System.getProperty("MyLog.level"));
	    //LOGGER.info("Logging an INFO-level message");
		 //LOGGER.setLevel(Level.FINEST);
		  String mylvl = "";
		  if (LOGGER.getLevel() !=null) {
			  	mylvl+=LOGGER.getLevel().toString();
		  }
		 System.out.println("Level="+mylvl);
	    LOGGER.fine("Logging an FINE-level message.zg");
	    System.out.println("End log");
	  }

}
