;MODULE PlaneMap OUTPUT A 0 I 1 O 0 C 0
;PROTO void PlaneMap(int2 coord, float value, image2d_t output);

void PlaneMap(int2 coord, float value, image2d_t output)
{
    uint color = ((value + 1.0f)/2.0f) * 255.0f ;
    write_imageui(output, coord, (uint4)(color, color, color, 255));
}

