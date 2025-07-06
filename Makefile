#
# Project Chimera - Makefile
#
# This Makefile manages the build process for a bare-metal RISC-V 64-bit OS.
# It handles compilation of C and Assembly source files, linking, and execution
# in the QEMU emulator.
#

# --- Toolchain and Target Configuration ---

# Set the prefix for the RISC-V cross-compiler toolchain.
RISCV_PREFIX := C:\Users\rimma\AppData\Roaming\xPacks\@xpack-dev-tools\riscv-none-elf-gcc\14.2.0-3.1\.content\bin\riscv-none-elf-

# Define the compiler, assembler, and linker commands.
CC := $(RISCV_PREFIX)gcc.exe
AS := $(RISCV_PREFIX)as.exe
LD := $(RISCV_PREFIX)ld.exe



# --- Build Directories and Files ---

# Source and object directories.
SRC_DIR := src
OBJ_DIR := obj

# Explicitly list all C source files
C_SOURCES   := $(SRC_DIR)/kernel.c \
               $(SRC_DIR)/mem.c \
               $(SRC_DIR)/uart.c \
               $(SRC_DIR)/trap_c.c

# Explicitly list all Assembly source files
S_SOURCES   := $(SRC_DIR)/boot.S \
               $(SRC_DIR)/trap.S \
               $(SRC_DIR)/switch_to_s_mode.S \
               $(SRC_DIR)/s_mode_stub.S

# Generate the corresponding object file names for C sources
C_OBJECTS   := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES))

# Define specific object file names for Assembly sources to avoid conflicts
BOOT_OBJ := $(OBJ_DIR)/boot.o
TRAP_ASM_OBJ := $(OBJ_DIR)/trap_asm.o
SWITCH_TO_S_MODE_OBJ := $(OBJ_DIR)/switch_to_s_mode.o
S_MODE_STUB_OBJ := $(OBJ_DIR)/s_mode_stub.o

# The final list of all object files to link.
# boot.o first, then other assembly objects, then all C objects.
OBJECTS     := $(BOOT_OBJ) $(TRAP_ASM_OBJ) $(SWITCH_TO_S_MODE_OBJ) $(S_MODE_STUB_OBJ) $(C_OBJECTS)

# The final executable file.
TARGET_ELF := chimera.elf

# --- Compiler and Linker Flags ---

# CFLAGS: Flags for the C compiler.
# -mcmodel=medany: Medium-any code model, suitable for position-independent code.
# -g: Generate debugging information.
# -Wall: Enable all warnings.
# -O2: Optimization level 2.
# -ffreestanding: No standard library, as we are on bare metal.
# -nostdlib: Do not link against the standard library.
CFLAGS := -mcmodel=medany -g -Wall -O2 -ffreestanding -nostdlib -march=rv64gc -mabi=lp64

# ASFLAGS: Flags for the assembler.
# -mcmodel=medany: Medium-any code model.
# -g: Generate debugging information.
ASFLAGS := -mcmodel=medany -g -march=rv64gc -mabi=lp64

# LDFLAGS: Flags for the linker.
# -T: Use the specified linker script.
LDFLAGS := -T $(SRC_DIR)/linker.ld -nostdlib -march=rv64gc -mabi=lp64

# --- QEMU Configuration ---

# QEMU command for running the kernel.
# -machine virt: Use the QEMU 'virt' machine model.
# -bios none: Do not use a default BIOS.
# -nographic: Disable graphical output, use the serial console.
# -kernel: Load the specified file as the kernel.
QEMU_CMD := C:\msys64\ucrt64\bin\qemu-system-riscv64.exe -machine virt -bios none -nographic -kernel $(TARGET_ELF)

# --- Build Rules ---

# The default target, 'all', depends on the final ELF file.
.PHONY: all
all: $(TARGET_ELF)

# Rule to link the final ELF file.
# Depends on all object files.
$(TARGET_ELF): $(OBJECTS)
	@echo "[LD] Linking..."
	@$(CC) $(LDFLAGS) -o $@ $(OBJECTS)
	@echo "[OK] Created executable: $(TARGET_ELF)"

# Rule to compile C source files into object files.
# Creates the object directory if it's not exist.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	@echo "[CC] Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule to assemble boot.S into boot.o
$(BOOT_OBJ): $(SRC_DIR)/boot.S
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	@echo "[AS] Assembling $<"
	@$(CC) $(ASFLAGS) -c $< -o $@

# Rule to assemble trap.S into trap_asm.o
$(TRAP_ASM_OBJ): $(SRC_DIR)/trap.S
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	@echo "[AS] Assembling $< to $(TRAP_ASM_OBJ)"
	@$(CC) $(ASFLAGS) -c $< -o $@



# Rule to assemble switch_to_s_mode.S into switch_to_s_mode.o
$(SWITCH_TO_S_MODE_OBJ): $(SRC_DIR)/switch_to_s_mode.S
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	@echo "[AS] Assembling $< to $(SWITCH_TO_S_MODE_OBJ)"
	@$(CC) $(ASFLAGS) -c $< -o $@

# Rule to assemble s_mode_stub.S into s_mode_stub.o
$(S_MODE_STUB_OBJ): $(SRC_DIR)/s_mode_stub.S
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	@echo "[AS] Assembling $< to $(S_MODE_STUB_OBJ)"
	@$(CC) $(ASFLAGS) -c $< -o $@

# --- Utility Rules ---

# Rule to run the OS in QEMU.
.PHONY: run
run: $(TARGET_ELF)
	@echo "[QEMU] Starting RISC-V machine..."
	@$(QEMU_CMD)

# Rule to clean up build artifacts.
.PHONY: clean
clean:
	@echo "[CLEAN] Removing build artifacts..."
	@rm -rf $(OBJ_DIR) $(TARGET_ELF)
	@echo "[OK] Clean complete."
