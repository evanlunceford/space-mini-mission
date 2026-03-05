#include <cstdio>
#include "time_system.h"


// ── TimelineNode ─────────────────────────────────────────────────────────────

TimelineNode::TimelineNode(std::string payload, time_t start, time_t end, TimelineNode* dep)
    : payload(std::move(payload)), start_time(start), end_time(end),
      is_completed(false), dependency(dep), next(nullptr)
{
    printf("[INFO] Created timeline node: '%s' [start=%ld, end=%ld, dependency=%s]\n",
        this->payload.c_str(), (long)start, (long)end,
        dep ? dep->payload.c_str() : "none");
}

bool TimelineNode::complete() {
    if (dependency && !dependency->is_completed) {
        printf("[WARN] complete: Cannot complete '%s' — dependency '%s' not yet completed.\n",
            payload.c_str(), dependency->payload.c_str());
        return false;
    }
    is_completed = true;
    return true;
}

bool TimelineNode::reset() {
    is_completed = false;
    return true;
}

bool TimelineNode::active_at(time_t time) const {
    return time >= start_time && time < end_time;
}

bool TimelineNode::add_dependency(TimelineNode* dep) {
    if (!dep) return false;
    dependency = dep;
    return true;
}


// ── Timeline ──────────────────────────────────────────────────────────────────

Timeline::Timeline(std::string name)
    : name(std::move(name)), head(nullptr)
{
    printf("[INFO] Created timeline: '%s'\n", this->name.c_str());
}

Timeline::~Timeline() {
    TimelineNode* current = head;
    while (current) {
        TimelineNode* next = current->next;
        delete current;
        current = next;
    }
}

Timeline::Timeline(Timeline&& other) noexcept
    : name(std::move(other.name)), head(other.head)
{
    other.head = nullptr;
}

Timeline& Timeline::operator=(Timeline&& other) noexcept {
    if (this != &other) {
        TimelineNode* current = head;
        while (current) {
            TimelineNode* next = current->next;
            delete current;
            current = next;
        }
        name = std::move(other.name);
        head = other.head;
        other.head = nullptr;
    }
    return *this;
}

bool Timeline::insert(TimelineNode* node) {
    if (!node) {
        printf("[ERROR] insert: Node not provided.\n");
        return false;
    }

    if (!head) {
        head = node;
        printf("[INFO] insert: Inserted '%s' as head of timeline '%s'.\n",
            node->payload.c_str(), name.c_str());
        return true;
    }

    if (node->start_time < head->start_time) {
        node->next = head;
        head = node;
        printf("[INFO] insert: Inserted '%s' before head of timeline '%s'.\n",
            node->payload.c_str(), name.c_str());
        return true;
    }

    TimelineNode* current = head;
    while (current->next && current->next->start_time <= node->start_time) {
        current = current->next;
    }

    node->next = current->next;
    current->next = node;

    printf("[INFO] insert: Inserted '%s' after '%s' in timeline '%s'.\n",
        node->payload.c_str(), current->payload.c_str(), name.c_str());
    return true;
}

bool Timeline::remove(TimelineNode* node) {
    if (!node) {
        printf("[ERROR] remove: Node not provided.\n");
        return false;
    }

    if (head == node) {
        head = node->next;
        delete node;
        return true;
    }

    TimelineNode* current = head;
    while (current->next && current->next != node) {
        current = current->next;
    }

    if (!current->next) {
        printf("[ERROR] remove: Node '%s' not found in timeline '%s'.\n",
            node->payload.c_str(), name.c_str());
        return false;
    }

    current->next = node->next;
    delete node;
    return true;
}

TimelineNode* Timeline::find_at(time_t time) const {
    TimelineNode* current = head;
    while (current) {
        if (current->start_time == time) return current;
        if (current->start_time > time) break;
        current = current->next;
    }
    printf("No timeline node found for time: %ld\n", (long)time);
    return nullptr;
}

TimelineNode* Timeline::next_event(time_t current_time) const {
    TimelineNode* current = head;
    while (current) {
        if (current->start_time > current_time) return current;
        current = current->next;
    }
    printf("No more events.\n");
    return nullptr;
}

bool Timeline::detect_conflicts() const {
    TimelineNode* current = head;
    while (current && current->next) {
        if (current->next->start_time < current->end_time) {
            printf("[WARN] detect_conflicts: Conflict in timeline '%s' — '%s' [ends=%ld] overlaps with '%s' [starts=%ld].\n",
                name.c_str(),
                current->payload.c_str(), (long)current->end_time,
                current->next->payload.c_str(), (long)current->next->start_time);
            return true;
        }
        current = current->next;
    }
    printf("[INFO] detect_conflicts: No conflicts in timeline '%s'.\n", name.c_str());
    return false;
}

void Timeline::print() const {
    printf("Timeline: %s\n", name.c_str());
    TimelineNode* current = head;
    while (current) {
        printf("Event: %s | Start: %ld | End: %ld | Completed: %s\n",
            current->payload.c_str(),
            (long)current->start_time,
            (long)current->end_time,
            current->is_completed ? "true" : "false");
        current = current->next;
    }
}

// ── Mission ───────────────────────────────────────────────────────────────────

Mission::Mission(std::string name)
    : name(std::move(name))
{
    printf("[INFO] Created mission: '%s'\n", this->name.c_str());
}

bool Mission::add_timeline(Timeline timeline) {
    timelines.push_back(std::move(timeline));
    printf("[INFO] add_timeline: Added timeline '%s' to mission '%s' (total: %zu).\n",
        timelines.back().name.c_str(), name.c_str(), timelines.size());
    return true;
}

Timeline* Mission::get_timeline(const std::string& timeline_name) {
    for (auto& t : timelines) {
        if (t.name == timeline_name) {
            printf("[INFO] get_timeline: Found '%s' in mission '%s'.\n",
                timeline_name.c_str(), name.c_str());
            return &t;
        }
    }
    printf("[ERROR] get_timeline: Timeline '%s' not found in mission '%s'.\n",
        timeline_name.c_str(), name.c_str());
    return nullptr;
}

TimelineNode* Mission::next_event(time_t current_time) {
    TimelineNode* best = nullptr;
    for (auto& t : timelines) {
        TimelineNode* candidate = t.next_event(current_time);
        if (candidate && (!best || candidate->start_time < best->start_time)) {
            best = candidate;
        }
    }
    if (best) {
        printf("[INFO] next_event: Next event in mission '%s' is '%s' at t=%ld.\n",
            name.c_str(), best->payload.c_str(), (long)best->start_time);
    } else {
        printf("[INFO] next_event: No upcoming events in mission '%s' after t=%ld.\n",
            name.c_str(), (long)current_time);
    }
    return best;
}

void Mission::events_at_time(time_t time) const {
    printf("Events at time %ld:\n", (long)time);
    for (const auto& t : timelines) {
        TimelineNode* node = t.head;
        while (node) {
            if (node->start_time == time) {
                printf("[%s] %s\n", t.name.c_str(), node->payload.c_str());
            }
            node = node->next;
        }
    }
}
