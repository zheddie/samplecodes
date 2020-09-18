package main
import "fmt"
import "calculation"
func main(){
	x,y :=15,10
	sum := calculation.Do_add(x,y)
	fmt.Println("Sum",sum)
}
