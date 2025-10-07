#include "top_level.h"

void frame_differencer(
    hls::stream<ap_uint<8>> &input_stream,
    hls::stream<ap_uint<8>> &output_stream,
    int width,
    int height
) {
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE s_axilite port=width bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=height bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=return bundle=CTRL

    // Utiliser une taille maximale fixe pour la BRAM
    static ap_uint<8> previous_frame[IMG_SIZE];
    #pragma HLS ARRAY_PARTITION variable=previous_frame cyclic factor=4 dim=1

    int idx = 0;

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            #pragma HLS PIPELINE II=1

            ap_uint<8> current_pixel = input_stream.read();
            ap_uint<8> diff;

            if(current_pixel > previous_frame[idx]) {
                diff = current_pixel - previous_frame[idx];
            } else {
                diff = previous_frame[idx] - current_pixel;
            }

            previous_frame[idx] = current_pixel;
            output_stream.write(diff);
            idx++;
        }
    }
}
