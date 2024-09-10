CXXFLAGS        = -O -Wall -Werror

MCP_SUM         = 0
MCP_PRINTTEXT   = 1
MCP_EVEN        = 2
MCP_RESYM       = 7
MCP_PRSYM       = 8
MCP_RANDOM      = 17
MCP_SETRANDOM   = 18
MCP_TIMEOFDAY   = 77

all:            resym.lib prsym.lib random.lib setrandom.lib

clean:
		rm -f *.o objfile

objfile:        objfile.cc

resym.lib:      objfile resym.src
		./objfile $(MCP_RESYM) < resym.src > $@

prsym.lib:      objfile prsym.src
		./objfile $(MCP_PRSYM) < prsym.src > $@

random.lib:     objfile random.src
		./objfile $(MCP_RANDOM) < random.src > $@

setrandom.lib:  objfile setrandom.src
		./objfile $(MCP_SETRANDOM) < setrandom.src > $@
