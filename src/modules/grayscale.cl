;MODULE grayscale FILTER
;OUTTYPE RGBA
;INPUT NOISE_VALUE FLOAT

    uint color = ((NOISE_VALUE + 1.0f)/2.0f) * 255.0f;
    return (uint4)(color, color, color, 255.0f);

;ENDMODULE
