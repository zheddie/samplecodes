#include <fcntl.h>
#include <stdio.h>
int main(){
	int amaster,aslave; 
	char * ttname;
	if ((amaster = open("/dev/ptc", O_RDWR | O_NOCTTY)) == -1)
		return (-1);
	for(int i =0;i<2;i++){
	if ((ttname = ttyname(amaster)) == NULL)
		return (-1);
	printf("ttname=%s\n",ttname);
	if ((aslave = open(ttname, O_RDWR | O_NOCTTY)) == -1) {
		close(amaster);
		return (-1);
	}
	printf("aslave=%d\n",aslave);
	}
	return (0);
}
