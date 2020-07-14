package main
import "fmt"
    
func display() {
	for i:=0; i<5; i++ {
		fmt.Println("In display")
	}
}

func main() {
	//invoking the goroutine display()
	go display()
	//The main() continues without waiting for display()
	for i:=0; i<5; i++ {
		fmt.Println("In main")
	}
}
