CXX := g++
CXXFLAGS := -Wall -Werror
LFLAGS := -L. -fpic -g

OBJECTS += $(patsubst %.cpp,%.o,$(SRC))
DEPENDENCIES := $(patsubst %.cpp,%.d,$(SRC))

default: release

debug: CXXFLAGS += -DDEBUG -O0 -g
debug: $(EXECUTABLE)

#release: CXXFLAGS += -DNDEBUG -O3 -g -ftree-vectorize
release: CXXFLAGS += -O3 -funroll-loops
release: $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CXX) $(LFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS) $(DEPENDENCIES)

# dependency rules
%.d: %.cpp
	@$(CXX) $(CXXFLAGS) -MM -MT '$@ $(@:.d=.o)' $< -o $@

-include $(DEPENDENCIES)