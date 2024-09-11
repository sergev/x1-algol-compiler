#
# List of procedures on the default library tape.
#
TARGET_LIB      = sum.lib resym.lib prsym.lib random.lib setrandom.lib

#
# All procedures compiled from assembly source.
#
ALL_LIB         = $(TARGET_LIB) even.lib printtext.lib timeofday.lib

CXXFLAGS        = -O -Wall -Werror

MCP_SUM         = 0
MCP_PRINTTEXT   = 1
MCP_EVEN        = 2
MCP_RESYM       = 7
MCP_PRSYM       = 8
MCP_RANDOM      = 17
MCP_SETRANDOM   = 18
MCP_TIMEOFDAY   = 77

all:            $(ALL_LIB) x1algol.lib

clean:
		rm -f *.o objfile

objfile:        objfile.cc

x1algol.lib:    $(TARGET_LIB)
		cat $(TARGET_LIB) > $@

sum.lib:        objfile sum.src
		./objfile $(MCP_SUM) < sum.src > $@

even.lib:       objfile even.src
		./objfile $(MCP_EVEN) < even.src > $@

printtext.lib:  objfile printtext.src
		./objfile $(MCP_PRINTTEXT) < printtext.src > $@

timeofday.lib:  objfile timeofday.src
		./objfile $(MCP_TIMEOFDAY) < timeofday.src > $@

resym.lib:      objfile resym.src
		./objfile $(MCP_RESYM) < resym.src > $@

prsym.lib:      objfile prsym.src
		./objfile $(MCP_PRSYM) < prsym.src > $@

random.lib:     objfile random.src
		./objfile $(MCP_RANDOM) < random.src > $@

setrandom.lib:  objfile setrandom.src
		./objfile $(MCP_SETRANDOM) < setrandom.src > $@
