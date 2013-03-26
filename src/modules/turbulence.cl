;MODULE turbulence FILTER
;OUTTYPE FLOAT
;ARG 0 float power 1.0 0.01 10.0
;INPUT INPUT FLOAT
;INPUT XDISTORT FLOAT
;INPUT YDISTORT FLOAT

    float x0 = pos.x + (12414.0 / 65536.0);
    float y0 = pos.y + (65124.0 / 65536.0);
    float x1 = pos.x + (26519.0 / 65536.0);
    float y1 = pos.y + (18128.0 / 65536.0);

    float2 orig_pos = pos;
    pos = (float2)(x0, y0);
    float xDistort = orig_pos.x + ( XDISTORT * power);
    pos = (float2)(x1, y1);
    float yDistort = orig_pos.y + ( YDISTORT * power);

    // Retrieve the output value at the offsetted input value instead of the
    // original input value.
    pos = (float2)(xDistort, yDistort);
    return INPUT;

;ENDMODULE
