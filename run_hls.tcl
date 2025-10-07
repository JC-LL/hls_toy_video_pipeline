# run_hls.tcl - Script pour Vitis HLS
set argc $argc
set argv $argv

# Configuration du projet
set project_name "motion_detection"
set top_function "motion_detection_system"
set solution_name "solution1"
set target_device "xc7z020-clg400-1"

# Fichiers sources
set src_files {
    "src/motion_detection.cpp"
    "src/image_producer.cpp"
    "src/frame_differencer.cpp"
    "src/motion_analyzer.cpp"
}

set testbench_files {
    "tb/testbench.cpp"
}

# Création du projet
open_project -reset $project_name
set_top $top_function
add_files $src_files
add_files -tb $testbench_files

# Solution
open_solution -reset $solution_name
set_part $target_device
create_clock -period 10 -name default

# Configuration selon l'argument
if {$argc > 0} {
    set command [lindex $argv 0]

    if {$command == "cosim"} {
        # Simulation C et RTL
        csim_design -clean
        csynth_design
        cosim_design -trace_level all
    } elseif {$command == "synth"} {
        # Synthèse complète
        csim_design -clean
        csynth_design
        export_design -format ip_catalog
    } else {
        # Simulation C seulement
        csim_design -clean
    }
} else {
    # Par défaut: simulation C
    csim_design -clean
}

exit
