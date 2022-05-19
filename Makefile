all: 
	cd src && $(MAKE) $@

clean:
	cd src && $(MAKE) $@
	rm -rf bin Debug Release x64 Setup64/{Debug,Release}
	@# consider adding .vs

new: clean all
