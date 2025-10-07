#include "top_level.h"

void motion_analyzer(
    hls::stream<ap_uint<8>> &input_stream,
    hls::stream<ap_uint<8>> &output_stream,
    hls::stream<ap_uint<16>> &motion_count_stream,
    int width,
    int height,
    ap_uint<8> threshold
) {
    #pragma HLS INTERFACE axis port=input_stream
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE axis port=motion_count_stream
    #pragma HLS INTERFACE s_axilite port=width bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=height bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=threshold bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=return bundle=CTRL

    ap_uint<16> motion_pixels = 0;

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            #pragma HLS PIPELINE II=1

            ap_uint<8> diff_pixel = input_stream.read();
            ap_uint<8> output_pixel;

            if(diff_pixel > threshold) {
                output_pixel = 255;
                motion_pixels++;
            } else {
                output_pixel = 0;
            }

            output_stream.write(output_pixel);
        }
    }

    motion_count_stream.write(motion_pixels);
}
