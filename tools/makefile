#
# Output files
#
TARGET = ./exporter

#
# Library sources
#
SRCS = exporter.cpp \
		../src/utility.c

#
# Compilation control
#
INCLUDES 	+=
DEFINES		+=

C_STD	= -std=c89
CXX_STD	= -std=c++98
WARNINGS	+=	 -Wall -Wextra -pedantic -Wshadow -Wpointer-arith \
				 -Wwrite-strings  -Wredundant-decls -Winline -Wno-long-long \
				 -Wuninitialized -Wconversion -Werror
CPPFLAGS += -MMD -MP $(DEFINES) $(INCLUDES) $(WARNINGS) -g
CFLAGS += $(CPPFLAGS) -Wmissing-declarations -Wstrict-prototypes -Wnested-externs -Wmissing-prototypes $(C_STD)
CXXFLAGS += $(CPPFLAGS) $(CXX_STD)

#############################################
OBJECTS = $(patsubst %.cpp,%.o,$(patsubst %.c,%.o,$(SRCS)))
############################################

ifndef V
	SILENT = @
endif

_DEPS := $(OBJECTS:.o=.d) $(TEST_OBJECTS:.o=.d)

.PHONY: clean

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	@echo "Linking $@..."
	$(SILENT) $(CXX) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

%.o : %.c
	@echo "Compiling $<..."
	$(SILENT) $(CC) $(CFLAGS) -c $< -o $@

%.o : %.cpp
	@echo "Compiling $<..."
	$(SILENT) $(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning..."
	$(SILENT) $(RM) -f -r $(OBJECTS) $(TEST_OBJECTS) $(_DEPS)
	$(SILENT) $(RM) $(LIBRARY) $(TARGET)

-include $(_DEPS)

