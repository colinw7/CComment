#.SILENT:

CPP_CC = g++
C_CC   = gcc

AR = ar
RM = rm

CDEBUG = -g

INC_DIR = ../include
OBJ_DIR = ../obj
LIB_DIR = ../lib

all: dirs $(LIB_DIR)/libCComment.a

dirs:
	@if [ ! -e ../obj ]; then mkdir ../obj; fi
	@if [ ! -e ../lib ]; then mkdir ../lib; fi
	@if [ ! -e ../bin ]; then mkdir ../bin; fi

CPP_SRC = \
CComment.cpp \
CCommentParser.cpp \
CConvertComment.cpp \

CPP_OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPP_SRC))

CPPFLAGS = \
-std=c++14 \
$(CDEBUG) \
-I$(INC_DIR) \
-I. \
-I../../CSpell/include \

clean:
	$(RM) -f $(OBJ_DIR)/*.o
	$(RM) -f $(LIB_DIR)/libCComment.a

.SUFFIXES: .cpp

$(CPP_OBJS): $(OBJ_DIR)/%.o: %.cpp
	$(CPP_CC) -c $< -o $(OBJ_DIR)/$*.o $(CPPFLAGS)

$(LIB_DIR)/libCComment.a: $(CPP_OBJS) $(C_OBJS)
	$(AR) crv $(LIB_DIR)/libCComment.a $(CPP_OBJS) $(C_OBJS)
