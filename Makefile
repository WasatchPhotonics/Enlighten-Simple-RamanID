all: 
	cd src && $(MAKE) $@

clean:
	cd src && $(MAKE) $@
	rm -rf bin Debug

new: clean all
