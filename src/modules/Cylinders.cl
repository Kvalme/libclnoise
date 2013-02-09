;MODULE Cylinders BASE A 1 I 0 O 1 C 0
;ARG 0 float frequency 1.0 0.01 100.0
;PROTO float Cylinders (float2 pos, float m_frequency);

float Cylinders (float2 pos, float m_frequency)
{
  float x = pos.x * m_frequency;
  float z = pos.y * m_frequency;

  float distFromCenter = sqrt (x * x + z * z);
  float distFromSmallerSphere = distFromCenter - floor (distFromCenter);
  float distFromLargerSphere = 1.0 - distFromSmallerSphere;
  float nearestDist = min (distFromSmallerSphere, distFromLargerSphere);
  return 1.0 - (nearestDist * 4.0); // Puts it in the -1.0 to +1.0 range.
}

