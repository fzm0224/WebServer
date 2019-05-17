LDFLAGS = -lpthread -lm
CPPFLAGS = -I ./include
SRCDIR = ./src
OBJDIR = ./obj

src = $(wildcard $(SRCDIR)/*.cpp)
obj = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(src))
target = ./bin/HttpServer

$(target):$(obj) 
	g++ $(obj) -o $@ $(CPPFLAGS) $(LDFLAGS)

$(OBJDIR)/%.o:$(SRCDIR)/%.cpp
	g++ -c $< -o $@ $(CPPFLAGS) $(LDFLAGS)

.PHONY:clean
clean:
	rm -f $(target) $(obj)
