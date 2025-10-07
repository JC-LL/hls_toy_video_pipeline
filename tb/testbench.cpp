#include <iostream>
#include <fstream>
#include <string>
#include "top_level.h"

using namespace std;

// Fonction pour sauvegarder une image en format PPM
void save_ppm(const string& filename, const ap_uint<8> pixels[], int width, int height) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return;
    }

    // En-tête PPM
    file << "P2" << endl;  // PGM format (grayscale)
    file << width << " " << height << endl;
    file << "255" << endl; // Max value

    // Données de l'image
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            file << (int)pixels[y * width + x] << " ";
        }
        file << endl;
    }

    file.close();
    cout << "Saved: " << filename << endl;
}

// Fonction pour capturer l'image du producteur (en la simulant)
void capture_producer_image(int frame, int width, int height) {
    // Recréer la logique du producteur pour générer la même image
    static int frame_count = 0;
    ap_uint<8> producer_image[IMG_SIZE];

    // Recréation de la logique du producteur
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            ap_uint<8> pixel;

            int sin_idx = (x + frame_count * 2) & (TRIG_LUT_SIZE-1);
            int cos_idx = (y + frame_count) & (TRIG_LUT_SIZE-1);

            int sin_val = sin_lut[sin_idx];
            int cos_val = cos_lut[cos_idx];
            int background = 128 + sin_val + cos_val;

            int obj1_x = (frame_count * 3) % width;
            int obj1_y = height / 3;

            int sin_idx2 = (frame_count * 6) & (TRIG_LUT_SIZE-1);
            int obj2_x = width / 2;
            int sin_val2 = sin_lut[sin_idx2];
            int obj2_y = (height / 2) + (sin_val2 / 4);

            if(background < 0) background = 0;
            if(background > 255) background = 255;

            pixel = (ap_uint<8>)background;

            if(x >= obj1_x && x < obj1_x + 8 && y >= obj1_y && y < obj1_y + 8) {
                pixel = 255;
            }

            if(x >= obj2_x && x < obj2_x + 6 && y >= obj2_y && y < obj2_y + 6) {
                pixel = 200;
            }

            int noise = ((x * 17 + y * 13 + frame_count * 11) % 7) - 3;
            int final_pixel = pixel.to_int() + noise;
            if(final_pixel < 0) final_pixel = 0;
            if(final_pixel > 255) final_pixel = 255;

            producer_image[y * width + x] = (ap_uint<8>)final_pixel;
        }
    }

    // Sauvegarder l'image du producteur
    string filename = "output/producer_frame_" + to_string(frame) + ".ppm";
    save_ppm(filename, producer_image, width, height);

    frame_count++;
}

int main() {
    hls::stream<ap_uint<8>> output_stream;
    hls::stream<ap_uint<16>> motion_count_stream;

    const int WIDTH = IMG_WIDTH;
    const int HEIGHT = IMG_HEIGHT;
    const ap_uint<8> THRESHOLD = 30;

    // Créer le dossier de sortie
    system("mkdir -p output");

    cout << "========================================" << endl;
    cout << "Testbench HLS Motion Detection System" << endl;
    cout << "Image size: " << WIDTH << "x" << HEIGHT << endl;
    cout << "Threshold: " << int(THRESHOLD) << endl;
    cout << "PPM files will be saved in ./output/" << endl;
    cout << "========================================" << endl;

    // Tableaux pour capturer les images intermédiaires
    ap_uint<8> raw_images[5][IMG_SIZE];  // 5 frames d'images brutes (pour démo)
    ap_uint<8> diff_images[5][IMG_SIZE]; // 5 frames d'images de différence
    ap_uint<8> output_images[5][IMG_SIZE]; // 5 frames de sortie

    for(int frame = 0; frame < 5; frame++) {
        cout << "\nProcessing frame " << frame << "..." << endl;

        // Capturer l'image du producteur (simulée)
        capture_producer_image(frame, WIDTH, HEIGHT);

        // Exécuter le système
        motion_detection_system(output_stream, motion_count_stream, WIDTH, HEIGHT, THRESHOLD);

        // Lire le compteur de mouvement
        ap_uint<16> motion_count = motion_count_stream.read();

        // Lire l'image de sortie finale
        for(int i = 0; i < IMG_SIZE; i++) {
            output_images[frame][i] = output_stream.read();
        }

        // Sauvegarder l'image de sortie (motion detection)
        string output_filename = "output/motion_frame_" + to_string(frame) + ".ppm";
        save_ppm(output_filename, output_images[frame], WIDTH, HEIGHT);

        cout << "Frame " << frame << ": " << motion_count << " motion pixels" << endl;
        cout << "Saved: " << output_filename << endl;

        // Vérification
        int white_pixels = 0;
        for(int i = 0; i < IMG_SIZE; i++) {
            if(output_images[frame][i] == 255) white_pixels++;
        }

        cout << "Verification: " << white_pixels << " white pixels in output" << endl;

        if(motion_count != white_pixels) {
            cout << "WARNING: Count mismatch!" << endl;
        }
    }

    // Générer un fichier de rapport
    ofstream report("output/simulation_report.txt");
    report << "HLS Motion Detection Simulation Report" << endl;
    report << "======================================" << endl;
    report << "Image size: " << WIDTH << "x" << HEIGHT << endl;
    report << "Threshold: " << int(THRESHOLD) << endl;
    report << "Frames processed: 5" << endl;
    report << endl;
    report << "Generated files:" << endl;
    report << "- producer_frame_X.ppm : Images générées par le producteur" << endl;
    report << "- motion_frame_X.ppm   : Résultats de détection de mouvement" << endl;
    report << endl;
    report << "To view images: convert output/*.ppm output/*.png && eog output/*.png" << endl;
    report.close();

    cout << "\n========================================" << endl;
    cout << "Simulation completed successfully!" << endl;
    cout << "Generated files in ./output/:" << endl;
    cout << "  - producer_frame_X.ppm : Input images" << endl;
    cout << "  - motion_frame_X.ppm   : Motion detection results" << endl;
    cout << "  - simulation_report.txt: Summary report" << endl;
    cout << endl;
    cout << "To view images, you can use:" << endl;
    cout << "  convert output/*.ppm output/*.png  # Convert to PNG" << endl;
    cout << "  eog output/*.png                   # View with Eye of GNOME" << endl;
    cout << "  or use any image viewer that supports PPM" << endl;
    cout << "========================================" << endl;

    return 0;
}
