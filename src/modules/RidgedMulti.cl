;MODULE RidgedMulti GENERATOR
;ARG 0 float frequency 1.0 0.01 100.0
;ARG 1 float lacunarity 2.0 0.01 10.0
;ARG 2 int octaveCount 6 1 10
;ARG 3 float persistence 0.5 0.01 10.0
;ARG 4 int seed 0 0 65535
;ARG 5 float offset 1.0 0.1 2.0
;ARG 6 float gain 2.0 0.1 4.0
;ARG 7 float spectralWeightExponent 1.0 0.1 5.0
;DEP GradientCoherentNoise3D
;DEP MakeInt32Range
;OUTTYPE FLOAT

  float x = pos.x * frequency;
  float y = frequency;
  float z = pos.y * frequency;

  float signal = 0.0f;
  float value  = 0.0f;
  float weight = 1.0f;

  float m_pSpectralWeights[30];
  float f = 1.0;
  for (int i = 0; i < octaveCount; ++i)
  {
    m_pSpectralWeights[i] = pow(f, -spectralWeightExponent);
    f *= lacunarity;
  }

  for (int curOctave = 0; curOctave < octaveCount; curOctave++) {

    // Make sure that these floating-point values have the same range as a 32-
    // bit integer so that we can pass them to the coherent-noise functions.
    float nx, ny, nz;
    nx = MakeInt32Range (x);
    ny = MakeInt32Range (y);
    nz = MakeInt32Range (z);

    // Get the coherent-noise value.
    int seed = (seed + curOctave) & 0x7fffffff;
    signal = GradientCoherentNoise3D (nx, ny, nz, seed);

    // Make the ridges.
    signal = fabs (signal);
    signal = offset - signal;

    // Square the signal to increase the sharpness of the ridges.
    signal *= signal;

    // The weighting from the previous octave is applied to the signal.
    // Larger values have higher weights, producing sharp points along the
    // ridges.
    signal *= weight;

    // Weight successive contributions by the previous signal.
    weight = signal * gain;
    weight = (weight > 1.0f)?1.0f:weight;
    weight = (weight < 0.0f)?0.0f:weight;

    // Add the signal to the output value.
    value += (signal * m_pSpectralWeights[curOctave]);

    // Go to the next octave.
    x *= lacunarity;
    y *= lacunarity;
    z *= lacunarity;
  }

  return (value * 1.25) - 1.0;

;ENDMODULE
