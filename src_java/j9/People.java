public class People{
	static int count=0;
	int age;
	String name;
	public static void main(String argv[]){
		People me = new People();
		me.setAge(40);
		me.setName("ZhangGan");
		System.out.println("Name:"+me.getName()+",Age:"+me.getAge()+",People.count:"+People.count);
		People.count++;
		System.out.println("Name:"+me.getName()+",Age:"+me.getAge()+",People.count:"+People.count);
	}
	People(){
		age = 10;
		name = "noname";
	}
	public void setAge(int a){
		age = a;
	}
	public void setName(String n){
		name = n;
	}
	public int getAge(){
		return(age);
	}
	public String getName(){
		return(name); 
	}
}
