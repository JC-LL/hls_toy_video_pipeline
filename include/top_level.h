#ifndef TOP_LEVEL_H
#define TOP_LEVEL_H

#include <hls_stream.h>
#include <ap_int.h>

// Utiliser des noms uniques pour éviter les conflits
#define IMG_WIDTH 64
#define IMG_HEIGHT 64
#define IMG_SIZE (IMG_WIDTH * IMG_HEIGHT)
#define TRIG_LUT_SIZE 64

extern ap_int<8> sin_lut[TRIG_LUT_SIZE];
extern ap_int<8> cos_lut[TRIG_LUT_SIZE];

void motion_detection_system(
    hls::stream<ap_uint<8>> &output_stream,
    hls::stream<ap_uint<16>> &motion_count_stream,
    int width,
    int height,
    ap_uint<8> threshold
);

void image_producer(
    hls::stream<ap_uint<8>> &output_stream,
    int width,
    int height
);

void frame_differencer(
    hls::stream<ap_uint<8>> &input_stream,
    hls::stream<ap_uint<8>> &output_stream,
    int width,
    int height
);

void motion_analyzer(
    hls::stream<ap_uint<8>> &input_stream,
    hls::stream<ap_uint<8>> &output_stream,
    hls::stream<ap_uint<16>> &motion_count_stream,
    int width,
    int height,
    ap_uint<8> threshold
);

ap_int<8> get_sin_value(int index);
ap_int<8> get_cos_value(int index);

#endif
