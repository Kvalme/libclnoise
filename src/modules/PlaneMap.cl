;MODULE PlaneMap OUTPUT
;OUTTYPE RGBA
;INPUT NOISE_VALUE FLOAT

    uint color = ((NOISE_VALUE + 1.0f)/2.0f) * 255.0f;
    write_imageui(output, coord, (uint4)(color, color, color, 255));

;ENDMODULE
