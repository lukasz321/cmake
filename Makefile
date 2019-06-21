BASE.DIR=$(PWD)
INSTALLED.HOST.DIR=$(BASE.DIR)/installed.host
BUILD.DIR=$(BASE.DIR)/build
SOURCE.DIR=$(BASE.DIR)/src

build: clean
	mkdir -p $(BUILD.DIR)
	mkdir -p $(INSTALLED.HOST.DIR)/lib
	cp $(SOURCE.DIR)/lib/64bit/lib* $(INSTALLED.HOST.DIR)/lib
	cd $(BUILD.DIR) && cmake -DCMAKE_PREFIX_PATH=$(INSTALLED.HOST.DIR) $(SOURCE.DIR)  && make install


run: .FORCE
	LD_LIBRARY_PATH=$(INSTALLED.HOST.DIR)/lib $(INSTALLED.HOST.DIR)/bin/pipelog

clean: .FORCE
	rm -rf $(BUILD.DIR)
	rm -rf $(INSTALLED.HOST.DIR)



.FORCE:
