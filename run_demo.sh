#!/bin/bash
# run_demo.sh - Script de démonstration HLS Motion Detection

echo "================================================"
echo "  Démonstration HLS Motion Detection System"
echo "================================================"

# Vérification de l'environnement
if [ -z "$XILINX_HLS" ]; then
    echo "Attention: XILINX_HLS non défini, utilisation de vitis_hls du PATH"
fi

# Création des dossiers
mkdir -p build reports data

echo "1. Nettoyage du projet..."
make clean

echo "2. Compilation du testbench standalone..."
make all

echo "3. Exécution de la simulation logicielle..."
echo "----------------------------------------"
make run
echo "----------------------------------------"

echo "4. Vérification des fichiers générés..."
ls -la build/

echo "5. Synthèse HLS (peut prendre quelques minutes)..."
make synth

echo "6. Génération des rapports..."
if [ -d "motion_detection/solution1/syn/report" ]; then
    cp motion_detection/solution1/syn/report/*.rpt reports/ 2>/dev/null || true
    echo "Rapports copiés dans: reports/"
fi

echo "================================================"
echo "Démonstration terminée avec succès!"
echo "Consultez les fichiers dans build/ et reports/"
echo "================================================"
