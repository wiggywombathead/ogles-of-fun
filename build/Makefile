CXX = g++

SRC = ../src
INC = ../include
OBJ = obj

_OBJS = $(wildcard $(SRC)/*.cpp)
OBJS = $(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(_OBJS))

CFLAGS = -g -std=c++17 -I$(INC)
LDFLAGS = -L/root/angle/out/Trace/ -lEGL -lGLESv2#`pkg-config --static --libs egl glesv2`

BIN = gltest

$(OBJ)/%.o: $(SRC)/%.cpp $(INC)/%.hpp
	@mkdir -p obj
	$(CXX) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.cpp
	@mkdir -p obj
	$(CXX) $(CFLAGS) -c $< -o $@ $(LDFLAGS)

$(BIN): $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(BIN) $(OBJ)
