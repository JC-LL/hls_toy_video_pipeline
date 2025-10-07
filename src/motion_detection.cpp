#include "top_level.h"

void motion_detection_system(
    hls::stream<ap_uint<8>> &output_stream,
    hls::stream<ap_uint<16>> &motion_count_stream,
    int width,
    int height,
    ap_uint<8> threshold
) {
    #pragma HLS INTERFACE axis port=output_stream
    #pragma HLS INTERFACE axis port=motion_count_stream
    #pragma HLS INTERFACE s_axilite port=width bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=height bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=threshold bundle=CTRL
    #pragma HLS INTERFACE s_axilite port=return bundle=CTRL
    #pragma HLS DATAFLOW

    hls::stream<ap_uint<8>> raw_image_stream;
    hls::stream<ap_uint<8>> diff_stream;
    #pragma HLS STREAM variable=raw_image_stream depth=32
    #pragma HLS STREAM variable=diff_stream depth=32

    image_producer(raw_image_stream, width, height);
    frame_differencer(raw_image_stream, diff_stream, width, height);
    motion_analyzer(diff_stream, output_stream, motion_count_stream, width, height, threshold);
}
