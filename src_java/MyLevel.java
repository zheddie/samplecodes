import java.util.logging.*;

public class MyLevel extends Level {
	public static final Level AUDIT = new MyLevel("AUDIT", 850);
	protected MyLevel(String arg0, int arg1) {
		super(arg0, arg1);
		// TODO Auto-generated constructor stub
	}

	 public static Level parse(String name) throws IllegalArgumentException {
		 if(name.equals(AUDIT.getName())){
			 return AUDIT;
		 }else{
			 return Level.parse(name);
		 }
	 }

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		//MyLevel ml = new MyLevel();
		Level s = MyLevel.AUDIT;
		Level t = MyLevel.parse("AUDIT");//
		System.out.println(s);
		System.out.println(t);
		if(s == t){
			System.out.println("Known Level");
		}
		else{
			System.out.println("Unknown Level");
		}
		
		Level s1 = MyLevel.OFF;
		Level t1 = MyLevel.parse("OFF");
		System.out.println(s1);
		System.out.println(t1);
		if(s1 == t1){
			System.out.println("Known Level");
		}
		else{
			System.out.println("Unknown Level");
		}			
		
	}

}
