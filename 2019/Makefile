CXXFLAGS=-g -Wall -std=c++17 -I./
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


intcode: intcode_test 11/painting 17/cleaner 19/drone 21/springdroid 09/base_pointer 23/network 15/oxygen 13/game

.PHONY: dep clean intcode
