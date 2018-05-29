CXX = g++
TARGET = wsmain
BUILD_DIR = ./build

RNG_TYPE=19937
#RNG_TYPE=2203

SRCS := $(wildcard *.cpp)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
OBJS := $(addprefix $(BUILD_DIR)/,$(OBJS))
DEPS := $(OBJS:.o=.d)

INC_DIRS :=
INC_FLAGS := $(addprefix -I ,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

CXXFLAGS = -O3 -DDSFMT_MEXP=$(RNG_TYPE)
# Uncomment the line below for debugging and comment out the above one
#CXXFLAGS = -g -DDSFMT_MEXP=$(RNG_TYPE)

LOADLIBES = -ljsoncpp -ldSFMT-$(RNG_TYPE)
LDLIBS =

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@ $(LOADLIBES) $(LDLIBS)

$(BUILD_DIR)/%.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR) $(TARGET)

#$(BUILD_DIR)/wsmain.o: wsmain.cpp wsnetwork.hpp wsnode.hpp wschannel.hpp
#	$(MKDIR_P) $(dir $@)
#	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

#$(BUILD_DIR)/wschannel.o: wschannel.cpp wschannel.hpp wsnode.hpp
#$(BUILD_DIR)/wsnetwork.o: wsnetwork.cpp wsnetwork.hpp wsnode.hpp wschannel.hpp
#$(BUILD_DIR)/wsnode.o: wsnode.cpp wsnode.hpp

-include $(DEPS)

MKDIR_P ?= mkdir -p
