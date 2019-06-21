BASE.DIR=$(PWD)
INSTALLED.HOST.DIR=$(BASE.DIR)/installed.host
BUILD.DIR=$(BASE.DIR)/build
SOURCE.DIR=$(BASE.DIR)/src
POWERSTRIP.DIR=$(BASE.DIR)/powerstrip-files

build: clean
	mkdir -p $(BUILD.DIR)
	mkdir -p $(INSTALLED.HOST.DIR)/lib
	mkdir -p $(INSTALLED.HOST.DIR)/bin
	cp -r $(POWERSTRIP.DIR)/lib/ $(INSTALLED.HOST.DIR)/lib  # for build time
	cp -r $(POWERSTRIP.DIR)/lib/ $(INSTALLED.HOST.DIR)/bin  # for runtime jankiness
	cp $(SOURCE.DIR)/lib/64bit/lib* $(INSTALLED.HOST.DIR)/lib
	cd $(BUILD.DIR) && cmake -DCMAKE_PREFIX_PATH=$(INSTALLED.HOST.DIR) -DCMAKE_INSTALL_PREFIX=$(INSTALLED.HOST.DIR) $(SOURCE.DIR)  && make install


run: .FORCE
	LD_LIBRARY_PATH=$(INSTALLED.HOST.DIR)/lib cd $(INSTALLED.HOST.DIR) && $(INSTALLED.HOST.DIR)/bin/pipelog

clean: .FORCE
	rm -rf $(BUILD.DIR)
	rm -rf $(INSTALLED.HOST.DIR)



.FORCE:
