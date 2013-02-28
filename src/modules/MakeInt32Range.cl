;MODULE MakeInt32Range FUNCTION
;PROTO float MakeInt32Range (float n);

float MakeInt32Range (float n)
{
    if (n >= 1073741824.0f) {
      return (2.0f * fmod (n, 1073741824.0f)) - 1073741824.0f;
    } else if (n <= -1073741824.0) {
      return (2.0f * fmod (n, 1073741824.0f)) + 1073741824.0f;
    } else {
      return n;
    }
}

;ENDMODULE
