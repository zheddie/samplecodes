#!/usr/bin/python3
# -*- coding: UTF-8 -*-
class Employee:
	'所有员工的基类'
	empCount = 0 
	def __init__(self, name, salary): 
		self.name = name 
		self.salary = salary 
		Employee.empCount += 1 
	def displayCount(self):
		print ("Total Employee %d" % Employee.empCount)
	def displayEmployee(self):
		print ("Name : ", self.name, ", Salary: ", self.salary)
		print ("Employee.__doc__:", Employee.__doc__)
		print ("Employee.__name__:", Employee.__name__)
		print ("Employee.__module__:", Employee.__module__)
		print ("Employee.__bases__:", Employee.__bases__)
		print ("Employee.__dict__:", Employee.__dict__)
	def __del__(self):
		print("in __del__ of Employee:id:",id(self))
if __name__=='__main__':
	emp = Employee('zg',2)
	print("id(emp)",id(emp))
	emp.displayCount()
	emp.displayEmployee()
	#del(emp)
	emp2 = Employee('zgg',3)
	print("emp2:id:",id(emp2))
