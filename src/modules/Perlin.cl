;MODULE Perlin GENERATOR
;ARG 0 float frequency 1.0 0.01 100.0
;ARG 1 float lacunarity 2.0 0.01 10.0
;ARG 2 int octaveCount 6 1 30
;ARG 3 float persistence 0.5 0.01 10.0
;ARG 4 int seed 0 0 65535
;DEP GradientCoherentNoise3D
;DEP MakeInt32Range
;OUTTYPE FLOAT

  float OUTPUT = 0.0;
  float signal = 0.0;
  float curPersistence = 1.0;
  float nx, ny, nz;

  float x = pos.x * freq;
  float y = 1.0 * freq;
  float z = pos.y * freq;

  for (int curOctave = 0; curOctave < octave; curOctave++) {

    // Make sure that these floating-point values have the same range as a 32-
    // bit integer so that we can pass them to the coherent-noise functions.
    nx = MakeInt32Range (x);
    ny = MakeInt32Range (y);
    ny = MakeInt32Range (z);

    // Get the coherent-noise value from the input value and add it to the
    // final result.
    seed = (seed + curOctave) & 0xffffffff;
    signal = GradientCoherentNoise3D (nx, ny, nz, seed);
    OUTPUT += signal * curPersistence;

    // Prepare the next octave.
    x *= lac;
    y *= lac;
    z *= lac;
    curPersistence *= pers;
  }

  return OUTPUT;

;ENDMODULE
