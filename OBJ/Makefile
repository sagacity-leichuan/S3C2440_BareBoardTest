
$(BIN_DIR)/$(BIN):$(objs_S) $(objs_c) $(hardware_obj_c)
	${LD} -T $(TOP_DIR)/$(LDS) $^ libgcc.a -o $(TEMP_DIR)/$(ELF) 
	#${LD} -Ttext 0 -Tdata 0xe80 $^ -o $(TEMP_DIR)/$(ELF)
	${OBJCOPY} -O binary -S $(TEMP_DIR)/$(ELF) $@
	${OBJDUMP} -D $(TEMP_DIR)/$(ELF) > $(TEMP_DIR)/$(DIS)