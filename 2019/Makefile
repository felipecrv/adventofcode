CXXFLAGS=-g -Wall -std=c++11 -I../
LDFLAGS=-lm

# Examples:
#
#     make 01/fuel_gold && ./a.out
#
#     make 01/fuel_gold && ./a.out < 01/in
#
#     make 01/fuel_gold && lldb ./a.out
#     > r < 01/in
#
%: %.cpp dep
	rm -f a.out
	clang++ $< -o a.out $(CXXFLAGS) $(LDFLAGS)

dep: # used to force re-compilation

clean:
	rm -rf a.out.dSYM
	rm -f a.out

.PHONY: dep clean
