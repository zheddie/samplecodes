
class Except{
	public static void main(String []argv){
		System.out.println("Trying in exception");
		try{
			throw new Exception("nothing done!");
		}catch(Exception e){
		e.printStackTrace();
		}
		System.out.println("After exception");
		return;
	}
}
