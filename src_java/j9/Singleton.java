public class Singleton{
	private static Singleton instance;
	public static void main(String[] args){
		Singleton.getInstance();
	}
	public static Singleton getInstance(){
		if (instance == null){
			synchronized (Singleton.class){
				if (instance == null){
					instance = new Singleton();

				}
			}
		}
		return instance;
	}
}
