;MODULE MIN FILTER
;OUTTYPE FLOAT
;INPUT VALUE1 FLOAT
;INPUT VALUE2 FLOAT

    return min((float)VALUE1, (float)VALUE2);

;ENDMODULE

;MODULE MAX FILTER
;OUTTYPE FLOAT
;INPUT VALUE1 FLOAT
;INPUT VALUE2 FLOAT

    return max((float)VALUE1, (float)VALUE2);

;ENDMODULE

;MODULE ABS FILTER
;OUTTYPE FLOAT
;INPUT VALUE FLOAT

    return fabs((float)VALUE);

;ENDMODULE

;MODULE CLAMP FILTER
;ARG 0 float MINVAL -1.0 -5.0 5.0
;ARG 1 float MAXVAL 1.0 -5.0 5.0
;INPUT VALUE FLOAT
;OUTTYPE FLOAT

    return clamp((float)VALUE, (float)MINVAL, (float)MAXVAL);

;ENDMODULE

;MODULE MIX FILTER
;INPUT X FLOAT
;INPUT Y FLOAT
;INPUT A FLOAT
;OUTTYPE FLOAT

    return mix((float)X, (float)Y, (float)A);

;ENDMODULE

;MODULE SMOOTHSTEP FILTER
;INPUT EDGE0 FLOAT
;INPUT EDGE1 FLOAT
;INPUT X FLOAT
;OUTTYPE FLOAT

    return smoothstep((float)EDGE0, (float)EDGE1, (float)X);

;ENDMODULE
