CXX = g++
CXXFLAGS = -std=c++17 -Iinclude
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRCS = main.cpp src/manager.cpp src/task.cpp src/utils.cpp src/logger.cpp src/task_factory.cpp src/gui_manager.cpp
OBJS = $(SRCS:.cpp=.o)

app.exe: $(SRCS)
	$(CXX) $(CXXFLAGS) -o app.exe $(SRCS) $(LDFLAGS)

clean:
	rm -f *.o app.exe2
