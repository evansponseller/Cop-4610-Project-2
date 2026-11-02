PARTS := part1 part2 part3
TESTS := part3/tests/elevator-test part3/tests/system-calls-test

all: $(PARTS)

$(PARTS):
	$(MAKE) -C $@

# the provided test makefiles rely on $(PWD), so enter each directory with cd
tests:
	for dir in $(TESTS); do (cd $$dir && $(MAKE)) || exit 1; done

clean:
	for dir in $(PARTS); do $(MAKE) -C $$dir clean; done
	-for dir in $(TESTS); do (cd $$dir && $(MAKE) clean); done

.PHONY: all $(PARTS) tests clean
