;MODULE SCurve5 FUNCTION
;PROTO float SCurve5 (float a)

float SCurve5 (float a)
{
    float a3 = a * a * a;
    float a4 = a3 * a;
    float a5 = a4 * a;
    return (6.0f * a5) - (15.0f * a4) + (10.0f * a3);
}

;ENDMODULE
