PARTS := part1 part2 part3

all: $(PARTS)

$(PARTS):
	$(MAKE) -C $@

tests:
	$(MAKE) -C part3/tests

clean:
	for dir in $(PARTS) part3/tests; do $(MAKE) -C $$dir clean; done

.PHONY: all $(PARTS) tests clean
