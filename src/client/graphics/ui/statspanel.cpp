#include "statspanel.h"

StatsPanel::StatsPanel() :
    recordIndex(0)
{ }

void StatsPanel::update(int64_t timeSinceLastFrame) {
    recordIndex = (recordIndex + 1) % NUM_RECORDS;

    fpsRecords[recordIndex] = 1000 / std::max(timeSinceLastFrame, (int64_t) 1);
}

void StatsPanel::draw(void) {
    ImGui::Begin("Stats");

    ImGui::PlotLines(
        "FPS", 
        fpsRecords, 
        IM_ARRAYSIZE(fpsRecords), 
        recordIndex, 
        std::format("Avg: {}", getAverageFPS()).c_str(), 
        0.0f, 
        1000.0f, 
        ImVec2(0, 80.0f)
    );

    ImGui::End();
}

int64_t StatsPanel::getAverageFPS(void) {
    int64_t average = 0;

    for(auto i = 0; i < IM_ARRAYSIZE(fpsRecords); i++) {
        average += fpsRecords[i];
    }

    return average / IM_ARRAYSIZE(fpsRecords);
}
