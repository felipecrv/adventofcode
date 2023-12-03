CXXFLAGS=-g -Wall -std=c++20 -I./
LDFLAGS=-lm

# Examples:
#
#     make 04/bingo && ./a.out
#
#     make 04/bingo && ./a.out < 04/in
#
#     make 04/bingo && lldb ./a.out
#     > settings set target.input-path 04/in
#     > r
#
%: %.cpp dep
	rm -f a.out
	clang++ $< -o a.out $(CXXFLAGS) $(LDFLAGS)

dep: # used to force re-compilation

clean:
	rm -rf a.out.dSYM
	rm -f a.out


.PHONY: dep clean
