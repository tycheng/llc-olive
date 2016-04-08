# Makefile variables
TEST_DIR	:=./testcases
BIN_ROOT	:=../../build/bin
TOOL_ROOT	:=../../build/tools
CC			:=$(BIN_ROOT)/clang
OLIVE		:=./olive/olive
EXE			:=$(BIN_ROOT)/llc-olive

NUM_REGS  	:= 3
# colorful terminal output
RED  =`tput setaf 1`
GREEN=`tput setaf 2`
RESET=`tput sgr0`

assembly:=$(patsubst %.c,%.s,$(wildcard $(TEST_DIR)/*.c))
bitcodes:=$(patsubst %.c,%.bc,$(wildcard $(TEST_DIR)/*.c))
executables:=$(patsubst %.c,%,$(wildcard $(TEST_DIR)/*.c))
executables+=$(patsubst %.c,%-expected,$(wildcard $(TEST_DIR)/*.c))

$(EXE): llc_olive.brg llc_olive.cpp FunctionState.cpp Tree.cpp RegisterAllocator.cpp
	(cd $(TOOL_ROOT); make -j6)

tar:
	(cd ../; tar -zcvf ./assignment6.tar.gz llc-olive/*)

%: $(EXE) ./testcases/%.s
	@echo -ne "${GREEN}[ ] TESTING $@ ... ${RESET}"
	@$(CC) ./testcases/$@.s -o ./testcases/$@ > /tmp/$@.compile
	@$(CC) ./testcases/$@.bc -o ./testcases/$@-expected >/dev/null 2>/dev/null
	@./test.sh $@

llc_olive.cpp: llc_olive.brg
	@$(OLIVE) $<

test: clean $(EXE) $(bitcodes) $(assembly) $(executables)

%.s: %.bc
	@$(EXE) --num_regs=$(NUM_REGS) $< -o $@

%.bc: %.c
	@$(CC) -O0 -emit-llvm $< -S -w -o $@

clean:
	@rm -rf $(EXE)
	@rm -rf $(executables)
	@rm -rf $(TEST_DIR)/*.s
	@rm -rf $(TEST_DIR)/*.log
	@rm -rf $(TEST_DIR)/.*.swp
	@rm -rf ./assignment6.tar.gz
	@rm -rf ./llc_olive.h ./llc_olive.cpp

.PHONY: opt compile tar test run push clean
