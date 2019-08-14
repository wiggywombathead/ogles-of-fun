CXX = g++

SRC = ../src
INC = ../include
OBJ = obj

_OBJS = $(wildcard $(SRC)/*.cpp)
OBJS = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(_OBJS))

CFLAGS = -g -std=c++17 -I$(INC)
LDFLAGS = -L/home/thomas/angle/out/Trace/ `pkg-config --static --libs egl glesv2`

DIRECTIVES = 

ifeq ($(MIP),tri)
	DIRECTIVES += -DMIP_TRILINEAR
else ifeq ($(MIP),bi)
	DIRECTIVES += -DMIP_BILINEAR
else ifeq ($(MIP),none)
	DIRECTIVES += -DMIP_NONE
endif

BIN = gltest

$(OBJ)/%.o: $(SRC)/%.cpp $(INC)/%.hpp
	@mkdir -p obj
	$(CXX) $(CFLAGS) -c $< -o $@ $(DIRECTIVES) $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p obj
	$(CXX) $(CFLAGS) -c $< -o $@ $(DIRECTIVES) $(LDFLAGS)

$(BIN): $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(BIN) $(OBJ)
