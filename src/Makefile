TARGET   = test
SRCS     = Test.cpp Helper.cpp Requestor.cpp Session.cpp 

CC       = g++
# CFLAGS   = -g -Werror
CFLAGS   = -g -Wall
INCLUDES = -I../include/ActiveTickServerAPI -I../include
LFLAGS   = -L../lib
LIBS     = -Wl,-Bstatic -lActiveTickServerAPI

OBJS     = $(SRCS:.cpp=.o)

all: $(TARGET)
	@echo compiling

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS) 

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^
