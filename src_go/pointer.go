package main
import "fmt"

func main() {
	a := 20
	b := 10
	fmt.Println("mainAddress:",main)
	fmt.Println("bAddress:",&b)
	fmt.Println("aAddress:",&a)
	fmt.Println("Value:",a)
}
