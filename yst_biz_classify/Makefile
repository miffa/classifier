#path
COMMON_PATH=$(HOME)/tools/trunk/src/common
BIN_PATH=$(HOME)/tools/trunk/bin
XMLDIR=$(HOME)/tools/trunk/src/tinyxml
LOGDIR=$(HOME)/tools/trunk/src/logtools

ACE_ROOT=
BUILD_DATE=$(shell date)

DEFS	   = -D__USE_UNIX98 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_BUILD_DATE="$(BUILD_DATE)" -D_DEBUG #-D_AVERAGE_PRO  
LIBPATH    = -L/usr/local/lib -L./lib -L. -L/usr/lib64

# Link the libyscommon.a staticly 
LIBS = -ldl -lpthread -lexpat #-lapr-1 -laprutil-1
APP_FLAG=-I$(COMMON_PATH) -I$(XMLDIR) -I$(LOGDIR)

#compile
CPP= g++ -g -W -Wall -m64 $(APP_FLAG)
CC= gcc -g $(APP_FLAG)

COMPILE_CC =$(CC)  $(DEFS)
COMPILE_CPP =$(CPP)  $(DEFS)
LINK        =$(CPP)  $(LIBPATH) 

# Add exe filename and source filename
YSS_TOOLS=yss_classifer

# Add target name
all:yss_classifer

#C++ SOURCE FILES
YSSXML_CPP=$(XMLDIR)/tinystr.C $(XMLDIR)/tinyxmlbase.C \
		$(XMLDIR)/tinyxmlnode.C $(XMLDIR)/tinyxmldocument.C \
		$(XMLDIR)/tinyxmldeclaration.C $(XMLDIR)/tinyxmlelement.C \
		$(XMLDIR)/tinyxmlattribute.C $(XMLDIR)/tinyxmlcomment.C \
		$(XMLDIR)/tinyxmltext.C $(XMLDIR)/tinyxmlunkown.C \
		$(XMLDIR)/tinyxmlhandle.C 
YSS_LOG_CC=$(LOGDIR)/logglobal.C
YSSCOMMON_CPP=$(COMMON_PATH)/YsGbkT2SConv.cpp $(COMMON_PATH)/YssCommon.cpp $(COMMON_PATH)/YssGbkUcsConv.cpp $(COMMON_PATH)/YsWordSegmentLR.cpp

YSS_TEST_CPP=YsHashData.cpp YsTrieTree.cpp YsDecisionTree.cpp YsTools.cpp YsAttributeRecord.cpp YsGetRecord.cpp YsBayesClassifer.cpp   YsCommonData.cpp  YsAdapter.cpp YsClassifierManager.cpp main.cpp

#C++ OBJECT FILES
YSSCOMMON_OBJ=$(YSSCOMMON_CPP:.cpp=.o)
YSS_TEST_OBJ=$(YSS_TEST_CPP:.cpp=.o)
YSS_XML_CPP_OBJ=$(YSSXML_CPP:.C=.o)
YSS_LOG_CC_OBJ=$(YSS_LOG_CC:.C=.o)

.SUFFIXES: .o .c .cpp .C

.c.o:
	@echo compiling $< "...\c"
	@$(COMPILE_CC) -o $@ -c $<
	@echo OK

.C.o:
	@echo compiling $< "...\c"
	@$(COMPILE_CPP) -o $@ -c $<
	@echo OK

.cpp.o:
	@echo compiling $< "...\c"
	@$(COMPILE_CPP) -o $@ -c $<
	@echo OK
	
	
# Add target and obj
yss_classifer:$(YSS_LOG_CC_OBJ) $(YSS_XML_CPP_OBJ) $(YSSCOMMON_OBJ) $(YSS_TEST_OBJ)
	#@g++ -o $(YSS_TOOLS) $(YSS_LOG_CC_OBJ) $(YSSCOMMON_OBJ) $(YSS_XML_CPP_OBJ) $(YSS_TEST_OBJ) $(APP_FLAG) $(LIBS)
	@$(LINK) $(YSS_LOG_CC_OBJ) $(YSS_XML_CPP_OBJ) $(YSSCOMMON_OBJ) $(YSS_TEST_OBJ) $(LIBS) -o $(YSS_TOOLS)
	@echo ========================
	@echo $(YSS_TOOLS) is built ok
# Add target
clean:
	@rm -f $(YSS_LOG_CC_OBJ) $(YSS_TOOLS) $(YSS_XML_CPP_OBJ) $(YSSCOMMON_OBJ) $(YSS_TEST_OBJ) core.*
	@echo clean ok
	
install:
	@[ -f $(BIN_PATH)/$(YSS_TOOLS) ]&& rm $(BIN_PATH)/$(YSS_TOOLS) || :
	@[ -f $(YSS_TOOLS) ] && mv $(YSS_TOOLS) $(BIN_PATH) && echo install ok || echo no $(YSS_TOOLS) is done, install error
