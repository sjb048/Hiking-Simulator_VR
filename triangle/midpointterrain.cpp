//
//  midpointterrain.cpp
//  triangle
//
//  Created by Sumaia Jahan Brinti on 27/11/2024.
//

#include "midpointterrain.h"
#include <cstdlib> // For rand()
#include <ctime>   // For time()

// Constructor
MidpointDispTerrain::MidpointDispTerrain()
{
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));
}
// Public method to create the terrain
void MidpointDispTerrain::CreateMidpointDisplacement(int TerrainSize, int PatchSize, float Roughness, float MinHeight, float MaxHeight)
{
    if (Roughness < 0.0f) {
        printf("%s: roughness must be positive - %f\n", __FUNCTION__, Roughness);
        exit(0);
    }

    m_terrainSize = TerrainSize;
    m_patchSize = PatchSize;

    SetMinMaxHeight(MinHeight, MaxHeight);

    // Initialize the height map
    m_heightMap.resize(m_terrainSize, std::vector<float>(m_terrainSize, 0.0f));

    // Set the corner values to random heights
    SetHeight(0, 0, RandomFloatRange(m_minHeight, m_maxHeight));
    SetHeight(m_terrainSize - 1, 0, RandomFloatRange(m_minHeight, m_maxHeight));
    SetHeight(0, m_terrainSize - 1, RandomFloatRange(m_minHeight, m_maxHeight));
    SetHeight(m_terrainSize - 1, m_terrainSize - 1, RandomFloatRange(m_minHeight, m_maxHeight));

    CreateMidpointDisplacementF32(Roughness);

    Normalize();

    Finalize();
}

// Get the height map
const std::vector<std::vector<float>>& MidpointDispTerrain::GetHeightMap() const
{
    return m_heightMap;
}

// Private methods
void MidpointDispTerrain::CreateMidpointDisplacementF32(float Roughness)
{
    int RectSize = m_terrainSize - 1;
    float CurHeight = static_cast<float>(RectSize) / 2.0f;
    float HeightReduce = pow(2.0f, -Roughness);

    while (RectSize > 1) {

        DiamondStep(RectSize, CurHeight);

        SquareStep(RectSize, CurHeight);

        RectSize /= 2;
        CurHeight *= HeightReduce;
    }
}

void MidpointDispTerrain::DiamondStep(int RectSize, float CurHeight)
{
    int HalfRectSize = RectSize / 2;

    for (int y = HalfRectSize; y < m_terrainSize; y += RectSize)
    {
        for (int x = HalfRectSize; x < m_terrainSize; x += RectSize)
        {
            float TopLeft     = GetHeight(x - HalfRectSize, y - HalfRectSize);
            float TopRight    = GetHeight(x + HalfRectSize, y - HalfRectSize);
            float BottomLeft  = GetHeight(x - HalfRectSize, y + HalfRectSize);
            float BottomRight = GetHeight(x + HalfRectSize, y + HalfRectSize);

            float Average = (TopLeft + TopRight + BottomLeft + BottomRight) / 4.0f;
            float Offset = RandomFloatRange(-CurHeight, CurHeight);

            SetHeight(x, y, Average + Offset);
        }
    }
}

void MidpointDispTerrain::SquareStep(int RectSize, float CurHeight)
{
    int HalfRectSize = RectSize / 2;

    for (int y = 0; y <= m_terrainSize - 1; y += HalfRectSize)
    {
        for (int x = (y + HalfRectSize) % RectSize; x <= m_terrainSize - 1; x += RectSize)
        {
            float Sum = 0.0f;
            int Count = 0;

            // Above
            if (y - HalfRectSize >= 0)
            {
                Sum += GetHeight(x, y - HalfRectSize);
                Count++;
            }
            // Below
            if (y + HalfRectSize < m_terrainSize)
            {
                Sum += GetHeight(x, y + HalfRectSize);
                Count++;
            }
            // Left
            if (x - HalfRectSize >= 0)
            {
                Sum += GetHeight(x - HalfRectSize, y);
                Count++;
            }
            // Right
            if (x + HalfRectSize < m_terrainSize)
            {
                Sum += GetHeight(x + HalfRectSize, y);
                Count++;
            }

            float Average = Sum / static_cast<float>(Count);
            float Offset = RandomFloatRange(-CurHeight, CurHeight);

            SetHeight(x, y, Average + Offset);
        }
    }
}

void MidpointDispTerrain::SetMinMaxHeight(float MinHeight, float MaxHeight)
{
    m_minHeight = MinHeight;
    m_maxHeight = MaxHeight;
}

void MidpointDispTerrain::Finalize()
{
    // Additional processing if needed
}

float MidpointDispTerrain::RandomFloatRange(float minValue, float maxValue)
{
    float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    float range = maxValue - minValue;
    return (random * range) + minValue;
}

int MidpointDispTerrain::CalcNextPowerOfTwo(int value)
{
    int power = 1;
    while (power < value)
        power <<= 1;
    return power;
}

void MidpointDispTerrain::Normalize()
{
    float minVal = std::numeric_limits<float>::max();
    float maxVal = std::numeric_limits<float>::lowest();

    // Find min and max values
    for (const auto& row : m_heightMap)
    {
        for (float val : row)
        {
            if (val < minVal) minVal = val;
            if (val > maxVal) maxVal = val;
        }
    }

    float range = maxVal - minVal;
    float targetRange = m_maxHeight - m_minHeight;

    // Normalize the values
    for (auto& row : m_heightMap)
    {
        for (float& val : row)
        {
            val = ((val - minVal) / range) * targetRange + m_minHeight;
        }
    }
}

float MidpointDispTerrain::GetHeight(int x, int y) const
{
    // Wrap around for seamless terrain
    x = (x + m_terrainSize) % m_terrainSize;
    y = (y + m_terrainSize) % m_terrainSize;
    return m_heightMap[y][x];
}

void MidpointDispTerrain::SetHeight(int x, int y, float value)
{
    // Wrap around for seamless terrain
    x = (x + m_terrainSize) % m_terrainSize;
    y = (y + m_terrainSize) % m_terrainSize;
    m_heightMap[y][x] = value;
}
