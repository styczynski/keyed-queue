
COMP_FLAGS=
LCOMP_FLAGS=

# Compilation flags
C_FLAGS=-Wall -Wextra -O2
CXX_FLAGS=-Wall -Wextra -std=c++17 -O2

# Paths and names generated
# from ./examples contents
EXAMPLES_LOCATIONS := $(wildcard ./examples/**)
EXAMPLES := $(foreach location,$(EXAMPLES_LOCATIONS),$(shell basename $(location)))
EXAMPLES_FILE_NAMES := $(EXAMPLES)
EXAMPLES_DIRS := $(addprefix ./examples/,$(addsuffix /,$(EXAMPLES)))
EXAMPLES_OUT_FILES := $(addprefix ./bin/,$(EXAMPLES_FILE_NAMES))
EXAMPLES_RUN_CMDS := $(addprefix run-,$(EXAMPLES))
EXAMPLES_RUN_CMDS := $(foreach runcmd,$(EXAMPLES_LOCATIONS),$(shell echo $(runcmd) | tr '_' '-'))

# Print help information
help:
	$(info Use the following targets:)
	$(info )
	$(info    all - to build all executables)
	$(info    [name] - to compile only one given example)
	$(info )
	$(info    clean - to remove the compilation files)
	$(info )
	$(info    run-[name] - to run exact executable. Available executions are:)
	$(foreach example,$(EXAMPLES),$(info        run-$(shell echo $(example) | tr '_' '-')))
	$(info )
	
# Build all examples
all: clean-log $(EXAMPLES_OUT_FILES)
	$(info [MAKE] Compilation done.)

# Cleans log file
clean-log:
	@echo "" > ./bin/log.txt
	
# Clean all compilation files
clean:
	$(shell rm -r -f -d ./bin/**)

./bin:
	@mkdir -p ./bin
	
# Template to generate
# ./example/[name]/[name] targets
# that compiles sources to executables
define compilation_template


ifeq ($(wildcard ./examples/$(1)/$(1).cc),) 

$(shell echo $(1) | tr '_' '-'): ./bin/$(1)
	$$(info [MAKE] Single example compiled.)

./bin/$(1).o: ./bin ./examples/$(1)/$(1).c
	$$(info [MAKE] Compiling example $(shell echo $(1) | tr '[:lower:]' '[:upper:]')... (GCC) )
	@gcc -c ./examples/$(1)/$(1).c -I ./src -o ./bin/$(1).o $(C_FLAGS) $(COMP_FLAGS)

./bin/$(1): ./bin ./bin/$(1).o
	$$(info [MAKE] Linking example $(shell echo $(1) | tr '[:lower:]' '[:upper:]')... )
	@g++ ./bin/$(1).o -o ./bin/$(1) $(LCOMP_FLAGS) || echo "" > ./bin/$(1)
 
else 

    $(shell echo $(1) | tr '_' '-'): ./bin/$(1)
	$$(info [MAKE] Single example compiled.)

./bin/$(1).o: ./bin ./examples/$(1)/$(1).cc
	$$(info [MAKE] Compiling example $(shell echo $(1) | tr '[:lower:]' '[:upper:]')... (G++) )
	@g++ -c ./examples/$(1)/$(1).cc -I ./src -o ./bin/$(1).o $(CXX_FLAGS) $(COMP_FLAGS)

./bin/$(1): ./bin ./bin/$(1).o
	$$(info [MAKE] Linking example $(shell echo $(1) | tr '[:lower:]' '[:upper:]')... )
	@g++ ./bin/$(1).o -o ./bin/$(1) $(LCOMP_FLAGS) || echo "" > ./bin/$(1)

endif

endef

# Template to generate
# run-[example] targets
# and [example] targets
# that runs compiled executable
define run_template

run-$(shell echo $(1) | tr '_' '-'): ./bin ./bin/$(1)
	$$(info [MAKE] Running example $(shell echo $(1) | tr '[:lower:]' '[:upper:]')...)
	$$(info )
	@"./bin/$(1)" && echo "[MAKE] Program exited."

endef

# Generate all the templates
$(foreach example, $(EXAMPLES), $(eval $(call run_template,$(example))))
$(foreach example_path, $(EXAMPLES), $(eval $(call compilation_template,$(example_path))))

.PHONY: all help clean $(EXAMPLES_RUN_CMDS)
