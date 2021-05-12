class myInt{
	int x;
	 void setInt(int i){
		x=i;
    }
     void  increaseInt(){
        x++;
    }
	 int  getInt(){return x;}
	 synchronized int getInt_inc(){
			int s;
			
				s = x;
				x++;

			return s;
		}
}
