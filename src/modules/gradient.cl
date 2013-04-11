;MODULE gradient FILTER
;ARG 0 gradient gradientPoints
;OUTTYPE RGBA
;INPUT NOISE_VALUE FLOAT

    int index = 0;
    float gradientPos = NOISE_VALUE;
    for(; index < GRAD_POINT_COUNT; ++index)
    {
	if (gradientPos < GRAD_POINT[index])break;
    }

    int index0 = clamp(index - 1, 0, GRAD_POINT_COUNT - 1);
    int index1 = clamp(index, 0, GRAD_POINT_COUNT - 1);
    
    float4 color0 = (float4)(GRAD_POINT_COLOR_R[index0], GRAD_POINT_COLOR_G[index0], GRAD_POINT_COLOR_B[index0], GRAD_POINT_COLOR_A[index0]);
    float4 color1 = (float4)(GRAD_POINT_COLOR_R[index1], GRAD_POINT_COLOR_G[index1], GRAD_POINT_COLOR_B[index1], GRAD_POINT_COLOR_A[index1]);

    float point0 = GRAD_POINT[index0];
    float point1 = GRAD_POINT[index1];
    
    float alpha = (gradientPos - point0) / (point1 - point0);

    uint4 color;
    color.x = (color1.x * alpha + (color0.x * (1.0f - alpha))) * 255.0f;
    color.y = (color1.y * alpha + (color0.y * (1.0f - alpha))) * 255.0f;
    color.z = (color1.z * alpha + (color0.z * (1.0f - alpha))) * 255.0f;
    color.w = (color1.w * alpha + (color0.w * (1.0f - alpha))) * 255.0f;

    return color;

;ENDMODULE
