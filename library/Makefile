CXXFLAGS        = -O -Wall -Werror

MCP_SUM         = 0
MCP_PRINTTEXT   = 1
MCP_EVEN        = 2
MCP_RESYM       = 7
MCP_RANDOM      = 17
MCP_SETRANDOM   = 18
MCP_TIMEOFDAY   = 77

all:            resym.lib

clean:
		rm -f *.o objfile

objfile:        objfile.cc

resym.lib:      objfile resym.src
		./objfile $(MCP_RESYM) < resym.src > $@
