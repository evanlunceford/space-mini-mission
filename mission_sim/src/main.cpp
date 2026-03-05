#include "core/time_system.h"

int main() {

    Mission mission("Test Mission");

    Timeline launch("Launch Timeline");

    TimelineNode* fuel = new TimelineNode("Fueling", 0, 10);
    TimelineNode* ignition = new TimelineNode("Engine Ignition", 10, 15, fuel);
    TimelineNode* liftoff = new TimelineNode("Liftoff", 15, 20, ignition);

    launch.insert(fuel);
    launch.insert(ignition);
    launch.insert(liftoff);

    mission.add_timeline(std::move(launch));


    mission.print();
    return 0;
}