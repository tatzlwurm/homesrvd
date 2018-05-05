

CXX		= g++
INC=-I.
LIB=-I./src/lib/jsoncpp/include
DEBUG		= -g
CXXFLAGS	= -Wall $(DEBUG) $(INC) $(LIB) -std=c++17 -pthread
LDFLAGS		= -pthread -lcurl 


vpath %.o ./obj/
OBJDIR	:= ./obj/

vpath %.cpp ./src/lib/jsoncpp/
OBJS_jason = json_writer.o json_value.o json_reader.o 

vpath %.cpp ./src/modules/netatmo/
OBJS_modules = netatmo.o 

vpath %.cpp ./src/
OBJS_main = homesrvd.o  

OBJS_temp = $(OBJS_jason) $(OBJS_modules) $(OBJS_main)
OBJS = $(addprefix $(OBJDIR), $(OBJS_temp) )

./obj/%.o : %.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

a: $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)


clean:
	rm $(OBJDIR)/*.o homesrvd 