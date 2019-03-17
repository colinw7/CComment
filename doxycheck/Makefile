CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = .
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CDoxyCheck

SRC = \
CDoxyCheck.cpp

OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC))

CPPFLAGS = \
-std=c++14 \
-I$(INC_DIR) \
-I.

LIBS = \
-L$(LIB_DIR) \
-L../../CStrUtil/lib \
-lCComment -lCStrUtil

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CDoxyCheck

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CDoxyCheck: $(OBJS) $(LIB_DIR)/libCComment.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CDoxyCheck $(OBJS) $(LIBS)
