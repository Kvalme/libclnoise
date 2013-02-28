;MODULE GradientCoherentNoise3D FUNCTION
;DEP GradientNoise3D
;DEP SCurve5
;PROTO float GradientCoherentNoise3D (float x, float y, float z, int seed);

float GradientCoherentNoise3D (float x, float y, float z, int seed)
{
  // Create a unit-length cube aligned along an integer boundary.  This cube
  // surrounds the input point.
  int x0 = (x > 0.0f? (int)x: (int)x - 1);
  int x1 = x0 + 1;
  int y0 = (y > 0.0f? (int)y: (int)y - 1);
  int y1 = y0 + 1;
  int z0 = (z > 0.0f? (int)z: (int)z - 1);
  int z1 = z0 + 1;

  // Map the difference between the coordinates of the input value and the
  // coordinates of the cube's outer-lower-left vertex onto an S-curve.
  float xs = 0.0f, ys = 0.0f, zs = 0.0f;
  xs = SCurve5 (x - (float)x0);
  ys = SCurve5 (y - (float)y0);
  zs = SCurve5 (z - (float)z0);

 // Now calculate the noise values at each vertex of the cube.  To generate
  // the coherent-noise value at the input point, interpolate these eight
  // noise values using the S-curve value as the interpolant (trilinear
  // interpolation.)
  float n0, n1, ix0, ix1, iy0, iy1;
  n0   = GradientNoise3D (x, y, z, x0, y0, z0, seed);
  n1   = GradientNoise3D (x, y, z, x1, y0, z0, seed);
  ix0  = mix (n0, n1, xs);
  n0   = GradientNoise3D (x, y, z, x0, y1, z0, seed);
  n1   = GradientNoise3D (x, y, z, x1, y1, z0, seed);
  ix1  = mix (n0, n1, xs);
  iy0  = mix (ix0, ix1, ys);
  n0   = GradientNoise3D (x, y, z, x0, y0, z1, seed);
  n1   = GradientNoise3D (x, y, z, x1, y0, z1, seed);
  ix0  = mix (n0, n1, xs);
  n0   = GradientNoise3D (x, y, z, x0, y1, z1, seed);
  n1   = GradientNoise3D (x, y, z, x1, y1, z1, seed);
  ix1  = mix (n0, n1, xs);
  iy1  = mix (ix0, ix1, ys);

  return mix (iy0, iy1, zs);
}

;ENDMODULE
