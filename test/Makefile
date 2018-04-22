CC = g++
RM = rm

CDEBUG = -g
LDEBUG = -g

INC_DIR = ../include
OBJ_DIR = .
LIB_DIR = ../lib
BIN_DIR = ../bin

all: $(BIN_DIR)/CComment $(BIN_DIR)/CConvertComment

SRC1 = \
CCommentMain.cpp

OBJS1 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC1))

SRC2 = \
CConvertCommentTest.cpp

OBJS2 = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRC2))

CPPFLAGS = \
-std=c++14 \
-I$(INC_DIR) \
-I.

LIBS = \
-L$(LIB_DIR) \
-L../../CSpell/lib \
-L../../CStrUtil/lib \
-lCComment -lCSpell -lCStrUtil

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(BIN_DIR)/CComment
	$(RM) -f $(BIN_DIR)/CConvertComment

.SUFFIXES: .cpp

.cpp.o:
	$(CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(BIN_DIR)/CComment: $(OBJS1) $(LIB_DIR)/libCComment.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CComment $(OBJS1) $(LIBS)

$(BIN_DIR)/CConvertComment: $(OBJS2) $(LIB_DIR)/libCComment.a
	$(CC) $(LDEBUG) -o $(BIN_DIR)/CConvertComment $(OBJS2) $(LIBS)
