;MODULE Checkerboard BASE A 0 I 0 O 1 C 0
;DEP MakeInt32Range
;PROTO float Checkerboard(float2 pos);

float Checkerboard(float2 pos)
{
    int ix = (int)(floor (MakeInt32Range (pos.x)));
//    int iy = (int)(floor (MakeInt32Range (1.0)));
    int iz = (int)(floor (MakeInt32Range (pos.y)));
    return (ix & 1 /*^ iy & 1*/ ^ iz & 1)? -1.0: 1.0;
}
