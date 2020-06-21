
public  class Main {
	public static void main(){
		fsub1 one = new fsub1("Sam");
		one.setStyle(5);
		asub2 two = (asub2)one;
		System.out.println("In Main.two.style:"+two.getStyle());
	}

}