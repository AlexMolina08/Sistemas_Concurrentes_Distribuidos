### usar con 'make x01', 'make x02' ..... 'make x12', o 'make xs13'

.SUFFIXES:
.PRECIOUS: ejemplo%_exe ejemplo%s_exe

isdarwin   := $(filter Darwin,$(shell uname -s))
compilador := $(if $(isdarwin), clang++, g++)
flags      := -std=c++11 -pthread -O3
BIN = $(shell pwd)/bin



x%: ejemplo%_exe
	./$(BIN)/$<

ejemplo%_exe: ejemplo%.cpp
	$(compilador) $(flags)  -o $(BIN)/$@ $<

xs%: $(BIN)/ejemplo%s_exe
	./$<

ejemplo%s_exe: ejemplo%-s.cpp
		$(compilador) $(flags)  -o $@ $< Semaphore.cpp

clean:
	rm -f $(BIN)/*_exe
