# run_hls_2020_correct.tcl - Gestion correcte des arguments
set project_name "motion_detection"
set top_function "motion_detection_system"

# Méthode robuste pour parser les arguments
set command "csim_only"  ;# Valeur par défaut

# Parcourir tous les arguments pour trouver "synth" ou "cosim"
for {set i 0} {$i < $argc} {incr i} {
    set arg [lindex $argv $i]
    if {$arg == "synth" || $arg == "cosim" || $arg == "csim_only"} {
        set command $arg
        break
    }
}

puts "INFO: Executing command: $command"

open_project -reset $project_name

# Ajouter les fichiers
add_files {
    src/motion_detection.cpp
    src/image_producer.cpp
    src/frame_differencer.cpp
    src/motion_analyzer.cpp
} -cflags "-I./include"

add_files -tb {
    tb/testbench.cpp
} -cflags "-I./include"

set_top $top_function
open_solution -reset "solution1"
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default

# Exécuter selon la commande détectée
if {$command == "csim_only"} {
    puts "INFO: Starting C simulation only..."
    csim_design -clean

} elseif {$command == "synth"} {
    puts "INFO: Starting C simulation..."
    csim_design -clean
    puts "INFO: Starting synthesis..."
    csynth_design

} elseif {$command == "cosim"} {
    puts "INFO: Starting C simulation..."
    csim_design -clean
    puts "INFO: Starting synthesis for co-simulation..."
    csynth_design
    puts "INFO: Starting co-simulation..."
    cosim_design -rtl verilog
    puts "SUCCESS: Co-simulation completed"
}

puts "INFO: HLS flow completed for command: $command"
exit
