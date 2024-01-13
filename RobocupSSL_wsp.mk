.PHONY: clean All

All:
	@echo ----------Building project:[ Strategy - Debug ]----------
	@cd "Strategy" && "$(MAKE)" -f "Strategy.mk"
clean:
	@echo ----------Cleaning project:[ Strategy - Debug ]----------
	@cd "Strategy" && "$(MAKE)" -f "Strategy.mk" clean
