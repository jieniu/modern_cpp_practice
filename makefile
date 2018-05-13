all : deduction_type

clean :
	rm -f deduction_type
	
deduction_type : deduction_type.cc
	g++ --std=c++14 -o deduction_type deduction_type.cc
