;MODULE Voronoi BASE A 4 I 0 O 1 C 0
;ARG 0 float frequency 1.0 0.01 100.0
;ARG 1 float displacement 1.0 0.01 100.0
;ARG 2 int seed 0 0 65535
;ARG 3 int enableDistance 0 0 1
;DEP GradientNoise3D
;DEP MakeInt32Range
;PROTO float Voronoi (float2 pos, float m_frequency, float m_displacement, int m_seed, int m_enableDistance);

float Voronoi (float2 pos, float m_frequency, float m_displacement, int m_seed, int m_enableDistance)
{
  // This method could be more efficient by caching the seed values.  Fix
  // later.

  float x = pos.x * m_frequency;
  float y = m_frequency;
  float z = pos.y * m_frequency;

  int xInt = (x > 0.0f? (int)x: (int)x - 1);
  int yInt = (y > 0.0f? (int)y: (int)y - 1);
  int zInt = (z > 0.0f? (int)z: (int)z - 1);

  float minDist = 2147483647.0f;
  float xCandidate = 0.0f;
  float yCandidate = 0.0f;
  float zCandidate = 0.0f;

  // Inside each unit cube, there is a seed point at a random position.  Go
  // through each of the nearby cubes until we find a cube with a seed point
  // that is closest to the specified position.
  for (int zCur = zInt - 2; zCur <= zInt + 2; zCur++) {
    for (int yCur = yInt - 2; yCur <= yInt + 2; yCur++) {
      for (int xCur = xInt - 2; xCur <= xInt + 2; xCur++) {

        // Calculate the position and distance to the seed point inside of
        // this unit cube.
        float xPos = xCur + ValueNoise3D (xCur, yCur, zCur, m_seed    );
        float yPos = yCur + ValueNoise3D (xCur, yCur, zCur, m_seed + 1);
        float zPos = zCur + ValueNoise3D (xCur, yCur, zCur, m_seed + 2);
        float xDist = xPos - x;
        float yDist = yPos - y;
        float zDist = zPos - z;
        float dist = xDist * xDist + yDist * yDist + zDist * zDist;

        if (dist < minDist) {
          // This seed point is closer to any others found so far, so record
          // this seed point.
          minDist = dist;
          xCandidate = xPos;
          yCandidate = yPos;
          zCandidate = zPos;
        }
      }
    }
  }

  float value;
  if (m_enableDistance) {
    // Determine the distance to the nearest seed point.
    float xDist = xCandidate - x;
    float yDist = yCandidate - y;
    float zDist = zCandidate - z;
    value = (sqrt (xDist * xDist + yDist * yDist + zDist * zDist)
      ) * 1.732051f - 1.0f;
  } else {
    value = 0.0;
  }

  // Return the calculated distance with the displacement value applied.
  return value + (m_displacement * (float)ValueNoise3D (
    (int)(floor (xCandidate)),
    (int)(floor (yCandidate)),
    (int)(floor (zCandidate)), m_seed));
}
