CC=gcc
CXX=g++
INC_PATH= ./Src

#PCAP_CFLAGS = -I/usr/local/include 
#PCAPLIB = -L/usr/local -lpfring -lpcap


O_FLAG = -O2
CFLAGS    += ${O_FLAG} -L $(PCAP_CFLAGS)-Wno-deprecated -Wall #-march=pentiumpro -Wall
LDFLAGS    = -L $(PCAPLIB) $(LIBLINEAR) -L/usr/lib -lnsl 
INCLUDE += 
CFLAGS += -I$(INC_PATH) $(INCLUDE) -g

# 输出文件名
TARGET= ../bin/upgrade
OUTPUT_PATH = ./Src/obj


#设置VPATH 包含源码的子目录列表
SUBDIR = .

#设置VPATH
SPACE =  
VPATH = $(subst $(SPACE),, $(strip $(foreach n,$(SUBDIR), $(INC_PATH)/$(n)))) $(OUTPUT_PATH)
CXX_SOURCES = $(notdir $(foreach n, $(SUBDIR), $(wildcard $(INC_PATH)/$(n)/*.cpp)))
CXX_OBJECTS = $(patsubst  %.cpp,  $(OUTPUT_PATH)/%.o, $(CXX_SOURCES))
#(if nids) OBJS		= $(NIDS)/checksum.o $(NIDS)/ip_fragment.o $(NIDS)/ip_options.o \
#		  $(NIDS)/libnids.o $(NIDS)/scan.o $(NIDS)/tcp.o $(NIDS)/util.o \
#			$(NIDS)/allpromisc.o $(NIDS)/hash.o
#OBJS = $(patsubst %.c, $(OUTPUT_PATH)/%.o, $(notdir $(wildcard $(BLAS_DIR)/*.c)))

$(TARGET):$(CXX_OBJECTS)
#(if nids) cd $(NIDS);make;cd ..
	#cd $(BLAS_DIR); make; cd -; cp $(BLAS_DIR)/*.o $(OUTPUT_PATH)
	$(CXX) -o $@ $(foreach n, $(CXX_OBJECTS), $(n)) $(foreach n, $(OBJS), $(n))  $(LDFLAGS) 
	#******************************************************************************#
	#                               Bulid successful !                             #
	#******************************************************************************#

$(OUTPUT_PATH)/%.o:%.cpp
	$(CXX) $< -c $(CFLAGS) -o $@

test:
#	@echo $(CXX_OBJECTS)
	@echo vpath $(VPATH)
	
mkdir:
	mkdir -p $(dir $(TARGET))
	mkdir -p $(OUTPUT_PATH)
	
rmdir:
	rm -rf $(dir $(TARGET))
	rm -rf $(OUTPUT_PATH)

clean:
	#(if nids)cd $(NIDS);make clean;cd ../../;
	#cd $(BLAS_DIR); make clean; cd -
	rm -f $(OUTPUT_PATH)/*
	rm -rf $(TARGET)
