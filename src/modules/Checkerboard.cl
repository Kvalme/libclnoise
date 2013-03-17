;MODULE Checkerboard GENERATOR
;DEP MakeInt32Range
;OUTTYPE FLOAT

    int ix = (int)(floor (MakeInt32Range (pos.x)));
//    int iy = (int)(floor (MakeInt32Range (1.0)));
    int iz = (int)(floor (MakeInt32Range (pos.y)));
    return (ix & 1 /*^ iy & 1*/ ^ iz & 1)? -1.0: 1.0;

;ENDMODULE
