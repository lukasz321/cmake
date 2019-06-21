BASE.DIR=$(PWD)
INSTALLED.HOST.DIR=$(BASE.DIR)/installed.host.dir
BUILD.DIR=$(BASE.DIR)/build
SOURCE.DIR=$(BASE.DIR)/src

clean: .FORCE
	rm -rf $(BUILD.DIR)
	rm -rf $(INSTALLED.HOST.DIR)

build: clean
	mkdir -p $(BUILD.DIR)
	mkdir -p $(INSTALLED.HOST.DIR)
	cp $(SOURCE.DIR)/lib/64bit/lib* $(INSTALLED.HOST.DIR)/lib
	cd $(BUILD.DIR) && cmake $(SOURCE.DIR) -DCMAKE_PREFIX_PATH=$(INSTALLED.HOST.DIR)/lib && make install


run: .FORCE
	LD_LIBRARY_PATH=$(INSTALLED.HOST.DIR)/lib $(INSTALLED.HOST.DIR)/bin/pipelog


.FORCE:
