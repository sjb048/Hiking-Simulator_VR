//
//  midpointterrain.h
//  triangle
//
//  Created by Sumaia Jahan Brinti on 27/11/2024.
//

#ifndef MIDPOINT_DISP_TERRAIN_H
#define MIDPOINT_DISP_TERRAIN_H

//#include "terrain.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <cstdio>

class MidpointDispTerrain
{
public:
    // Public methods
    // Constructor
        MidpointDispTerrain();

        // Public methods
        void CreateMidpointDisplacement(int TerrainSize, int PatchSize, float Roughness, float MinHeight, float MaxHeight);

        // Get the height map
        const std::vector<std::vector<float>>& GetHeightMap() const;

    private:
        // Private methods
        void CreateMidpointDisplacementF32(float Roughness);
        void DiamondStep(int RectSize, float CurHeight);
        void SquareStep(int RectSize, float CurHeight);
        void SetMinMaxHeight(float MinHeight, float MaxHeight);
        void Finalize();
        float RandomFloatRange(float minValue, float maxValue);
        int CalcNextPowerOfTwo(int value);
        void Normalize();

        // Height map access methods
        float GetHeight(int x, int y) const;
        void SetHeight(int x, int y, float value);

        // Member variables
        int m_terrainSize;
        int m_patchSize;
        float m_minHeight;
        float m_maxHeight;
        std::vector<std::vector<float>> m_heightMap;
};
#endif
