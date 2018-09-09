
# output path
BIN = ./bin
SRC = ./src
TEST= ./test

# debug stuff 
ifeq ($(CFLAG), DEBUG)
	CFLAGS += -g
endif

AR  = ar -cqs
# itermidiate objects
OBJ = $(addprefix $(BIN)/, \
	  md5.o \
	  util_rbtree.o  \
	  conhash_inter.o \
	  conhash_util.o  \
	  conhash.o \
	  )

SAMPLE_OBJS = $(addprefix $(BIN)/, \
		sample.o \
		)

TEST_OBJS = $(addprefix $(BIN)/, \
		test.o \
		)
		
# include file path
INC = -I$(SRC)

TARGETS = $(BIN)/libconhash.a $(BIN)/sample $(BIN)/test
 
all: clean prepare $(TARGETS)

# build libconhash as a static lib 
$(BIN)/libconhash.a: $(OBJ) 
	$(AR) $@ $(OBJ)
	
# build sample
$(BIN)/sample: $(SAMPLE_OBJS)
	gcc -O -o $@ $< -L. -L$(BIN) -lconhash
	
$(BIN)/%.o: $(SRC)/%.c
	gcc $(INC) $(CFLAGS) -c $< -o $@	

$(BIN)/test: $(TEST_OBJS)
	g++ -O -o $@ $< -L. -L$(BIN) -lconhash

$(BIN)/%.o: $(TEST)/%.cpp
	g++ $(INC) $(CFLAGS) -c $< -o $@
	
# prepare the bin dir	
prepare: 
		-mkdir $(BIN)
	  
clean:
		-rm -rf $(BIN)

.PHONY: all prepare clean
