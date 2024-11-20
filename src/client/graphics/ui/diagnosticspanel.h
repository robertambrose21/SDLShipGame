#pragma once

#include <cstdint>
#include <algorithm>
#include <format>

#include "imgui.h"

class DiagnosticsPanel {
private:
    static const int NUM_RECORDS = 600;

    float fpsRecords[NUM_RECORDS];
    int recordIndex;

    int64_t getAverageFPS(void);

public:
    DiagnosticsPanel();

    void update(int64_t timeSinceLastFrame);
    void draw(void);
};