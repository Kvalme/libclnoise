;MODULE PlaneMap OUTPUT A 0 I 1 O 0 C 0

uint color = (input0 + 1.0) * 255.0;
write_imageui(output, coord, (uint4)(color, color, color, 255));

