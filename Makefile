NAME		= ircserv

CXX			= c++
CXXFLAGS	= -Wall -Wextra -Werror -std=c++98 -Iincludes

SRCS_DIR	= srcs
OBJS_DIR	= obj

SRCS		= main.cpp \
			  parsing.cpp \
			  server.cpp \
			  client.cpp \
			  channel.cpp \
			  commands/invite.cpp \
			  commands/join.cpp \
			  commands/kick.cpp \
			  commands/mode.cpp \
			  commands/nick.cpp \
			  commands/part.cpp \
			  commands/pass.cpp \
			  commands/ping.cpp \
			  commands/privmsg.cpp \
			  commands/quit.cpp \
			  commands/topic.cpp \
			  commands/user.cpp

OBJS		= $(addprefix $(OBJS_DIR)/, $(SRCS:.cpp=.o))
DEPS		= $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re