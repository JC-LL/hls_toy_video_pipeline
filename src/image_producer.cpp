#include "top_level.h"

ap_int<8> sin_lut[TRIG_LUT_SIZE] = {
    0, 12, 24, 36, 47, 58, 67, 75, 81, 85, 87, 88, 86, 82, 76, 68,
    58, 47, 36, 24, 12, 0, -12, -24, -36, -47, -58, -67, -75, -81, -85, -87,
    -88, -86, -82, -76, -68, -58, -47, -36, -24, -12, 0, 12, 24, 36, 47, 58,
    67, 75, 81, 85, 87, 88, 86, 82, 76, 68, 58, 47, 36, 24, 12, 0
};

ap_int<8> cos_lut[TRIG_LUT_SIZE] = {
    88, 86, 82, 76, 68, 58, 47, 36, 24, 12, 0, -12, -24, -36, -47, -58,
    -67, -75, -81, -85, -87, -88, -86, -82, -76, -68, -58, -47, -36, -24, -12, 0,
    12, 24, 36, 47, 58, 67, 75, 81, 85, 87, 88, 86, 82, 76, 68, 58,
    47, 36, 24, 12, 0, -12, -24, -36, -47, -58, -67, -75, -81, -85, -87, -88
};

ap_int<8> get_sin_value(int index) {
    #pragma HLS INLINE
    return sin_lut[index & (TRIG_LUT_SIZE-1)];
}

ap_int<8> get_cos_value(int index) {
    #pragma HLS INLINE
    return cos_lut[index & (TRIG_LUT_SIZE-1)];
}

void image_producer(
    hls::stream<ap_uint<8>> &output_stream,
    int width,
    int height
) {
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=width bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=height bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=return bundle=CTRL

    static int frame_count = 0;

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            #pragma HLS PIPELINE II=1

            ap_uint<8> pixel;

            // Convertir en int avant les opérations arithmétiques
            int sin_idx = (x + frame_count * 2) & (TRIG_LUT_SIZE-1);
            int cos_idx = (y + frame_count) & (TRIG_LUT_SIZE-1);

            // Extraire les valeurs et convertir en int
            int sin_val = get_sin_value(sin_idx).to_int();
            int cos_val = get_cos_value(cos_idx).to_int();
            int background = 128 + sin_val + cos_val;

            int obj1_x = (frame_count * 3) % width;
            int obj1_y = height / 3;

            int sin_idx2 = (frame_count * 6) & (TRIG_LUT_SIZE-1);
            int obj2_x = width / 2;

            // Convertir en int pour l'opération de division
            int sin_val2 = get_sin_value(sin_idx2).to_int();
            int obj2_y = (height / 2) + (sin_val2 / 4);

            // Assurer que background est dans la plage 0-255
            if(background < 0) background = 0;
            if(background > 255) background = 255;

            pixel = (ap_uint<8>)background;

            // Premier objet (carré blanc)
            if(x >= obj1_x && x < obj1_x + 8 && y >= obj1_y && y < obj1_y + 8) {
                pixel = 255;
            }

            // Second objet (carré clair)
            if(x >= obj2_x && x < obj2_x + 6 && y >= obj2_y && y < obj2_y + 6) {
                pixel = 200;
            }

            // Bruit synthétisable
            int noise = ((x * 17 + y * 13 + frame_count * 11) % 7) - 3;
            int final_pixel = pixel.to_int() + noise;

            // Saturation
            if(final_pixel < 0) final_pixel = 0;
            if(final_pixel > 255) final_pixel = 255;

            output_stream.write((ap_uint<8>)final_pixel);
        }
    }
    frame_count++;
}
