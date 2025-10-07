# Makefile for HLS Motion Detection Testbench
# Usage: make [all|clean|run|cosim|synth]

# Configuration
PROJECT_NAME = motion_detection
TB_EXECUTABLE = testbench
HLS_COMMAND = vitis_hls

# Fichiers sources
SOURCES = src/motion_detection.cpp \
          src/image_producer.cpp \
          src/frame_differencer.cpp \
          src/motion_analyzer.cpp

TESTBENCH = tb/testbench.cpp

# Dossiers
BUILD_DIR = build
REPORT_DIR = reports
DATA_DIR = data
OUTPUT_DIR = output

# Flags de compilation
CXX = g++
CXXFLAGS = -O2 -std=c++11 -I./include -I${XILINX_HLS}/include
LDFLAGS = -lm

# Cibles principales
.PHONY: all clean run cosim synth help

all: $(BUILD_DIR)/$(TB_EXECUTABLE)

# Création des dossiers
$(BUILD_DIR) $(REPORT_DIR) $(DATA_DIR):
	@mkdir -p $@

# Compilation du testbench standalone
$(BUILD_DIR)/$(TB_EXECUTABLE): $(SOURCES) $(TESTBENCH) | $(BUILD_DIR)
	@echo "Compiling testbench..."
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(TESTBENCH) $(LDFLAGS)
	@echo "Testbench compiled: $@"

# Exécution du testbench
run: $(BUILD_DIR)/$(TB_EXECUTABLE)
	@echo "Running testbench..."
	@cd $(BUILD_DIR) && ./$(TB_EXECUTABLE)
	@echo "Testbench execution completed."

# Simulation Cosim avec Vitis HLS
cosim: | $(BUILD_DIR) $(REPORT_DIR)
	@echo "Running Co-simulation with Vitis HLS..."
	$(HLS_COMMAND) -f run_hls.tcl -tclargs cosim
	@echo "Co-simulation completed."

# Synthèse HLS
synth: | $(BUILD_DIR) $(REPORT_DIR)
	@echo "Running HLS Synthesis..."
	$(HLS_COMMAND) -f run_hls.tcl -tclargs synth
	@echo "Synthesis completed."

deps:
	@echo "Installing dependencies for image conversion..."
	sudo apt-get update && sudo apt-get install -y imagemagick
	@echo "Dependencies installed."

view: run
	@echo "Converting PPM files to PNG..."
	cd output && convert *.ppm *.png 2>/dev/null || true
	@echo "Conversion completed. Check output/*.png files"

# Nettoyage
clean:
	@echo "Cleaning project..."
	rm -rf $(BUILD_DIR) $(REPORT_DIR) $(DATA_DIR) $(OUTPUT_DIR)
	rm -rf *.log *.jou vivado_hls.log
	rm -rf $(PROJECT_NAME)
	@echo "Clean completed."

# Aide
help:
	@echo "Available targets:"
	@echo "  all     - Compile testbench executable"
	@echo "  run     - Run testbench simulation"
	@echo "  cosim   - Run Vitis HLS co-simulation"
	@echo "  synth   - Run HLS synthesis"
	@echo "  clean   - Clean all generated files"
	@echo "  help    - Show this help message"

# Dépendances
include/top_level.h include/stream_utils.h:
	@mkdir -p include

src/%.cpp: include/top_level.h
	@mkdir -p src

tb/%.cpp: include/top_level.h
	@mkdir -p tb
