# Makefile pour Vitis HLS 2020
PROJECT_NAME = motion_detection
TB_EXECUTABLE = testbench
HLS_COMMAND = vitis_hls

SOURCES = src/motion_detection.cpp \
          src/image_producer.cpp \
          src/frame_differencer.cpp \
          src/motion_analyzer.cpp

TESTBENCH = tb/testbench.cpp

BUILD_DIR = build
REPORT_DIR = reports

CXX = g++
CXXFLAGS = -O2 -std=c++11 -I./include -I${XILINX_HLS}/include
LDFLAGS = -lm

# Cibles principales
.PHONY: all clean run hls-synth hls-csim hls-cosim

all: $(BUILD_DIR)/$(TB_EXECUTABLE)

$(BUILD_DIR)/$(TB_EXECUTABLE): $(SOURCES) $(TESTBENCH)
	@mkdir -p $(BUILD_DIR)
	@echo "Compiling software testbench..."
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(TESTBENCH) $(LDFLAGS)

run: $(BUILD_DIR)/$(TB_EXECUTABLE)
	@echo "Running software simulation..."
	@./$(BUILD_DIR)/$(TB_EXECUTABLE)

# Cibles HLS pour Vitis 2020
hls-csim:
	@echo "Running HLS C simulation (Vitis 2020)..."
	$(HLS_COMMAND) -f run_hls_2020.tcl -tclargs dummy csim_only dummy

hls-synth:
	$(HLS_COMMAND) -f run_hls_2020.tcl -tclargs dummy synth dummy

hls-cosim:
	@echo "Running HLS Co-simulation (Vitis 2020)..."
	$(HLS_COMMAND) -f run_hls_2020.tcl -tclargs dummy cosim dummy

hls-clean:
	@echo "Cleaning HLS project..."
	rm -rf $(PROJECT_NAME) *.log *.jou

clean: hls-clean
	rm -rf $(BUILD_DIR) $(REPORT_DIR) output

# Diagnostic
debug:
	@echo "=== Vitis HLS 2020 Environment ==="
	@vitis_hls -version | head -2
	@echo "XILINX_HLS: ${XILINX_HLS}"
	@echo "Project files:"
	@find src tb include -name "*.cpp" -o -name "*.h" 2>/dev/null | sort

help:
	@echo "Targets for Vitis HLS 2020:"
	@echo "  run         - Run software testbench"
	@echo "  hls-csim    - HLS C simulation only"
	@echo "  hls-synth   - HLS synthesis"
	@echo "  hls-cosim   - HLS co-simulation"
	@echo "  debug       - Show environment info"
