;MODULE Cylinders GENERATOR
;ARG 0 float frequency 1.0 0.01 100.0
;OUTTYPE FLOAT

  float x = pos.x * frequency;
  float z = pos.y * frequency;

  float distFromCenter = sqrt (x * x + z * z);
  float distFromSmallerSphere = distFromCenter - floor (distFromCenter);
  float distFromLargerSphere = 1.0 - distFromSmallerSphere;
  float nearestDist = min (distFromSmallerSphere, distFromLargerSphere);
  return 1.0 - (nearestDist * 4.0); // Puts it in the -1.0 to +1.0 range.

;ENDMODULE
