class zgtest{
	int age;
	public static void main(String[] argv){
		zgtest z1 = new zgtest();
		z1.setage(10);
		System.out.println("hello!!!");
		System.out.println("Age="+z1.getage());
	}
	public void setage(int x){
		age = x;
	}
	public int getage(){
		return(age);
	}
}
