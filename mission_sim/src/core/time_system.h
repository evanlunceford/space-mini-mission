#ifndef TIME_SYSTEM_H
#define TIME_SYSTEM_H

#include <ctime>
#include <string>
#include <vector>


class TimelineNode {
public:
    std::string payload;
    time_t start_time;
    time_t end_time;
    bool is_completed;

    // If nullptr, node has no dependency
    TimelineNode* dependency;

    TimelineNode* next;

    TimelineNode(std::string payload, time_t start, time_t end, TimelineNode* dep = nullptr);

    bool complete();
    bool reset();
    bool active_at(time_t time) const;
    bool add_dependency(TimelineNode* dep);
};


class Timeline {
public:
    std::string name;
    TimelineNode* head;

    explicit Timeline(std::string name);
    ~Timeline();

    // Timelines own their nodes — disable copy, allow move
    Timeline(const Timeline&) = delete;
    Timeline& operator=(const Timeline&) = delete;
    Timeline(Timeline&& other) noexcept;
    Timeline& operator=(Timeline&& other) noexcept;

    bool insert(TimelineNode* node);
    bool remove(TimelineNode* node);
    TimelineNode* find_at(time_t time) const;
    TimelineNode* next_event(time_t current_time) const;
    bool detect_conflicts() const;
    void print() const;
};


class Mission {
public:
    std::string name;
    std::vector<Timeline> timelines;

    explicit Mission(std::string name);

    bool add_timeline(Timeline timeline);
    Timeline* get_timeline(const std::string& timeline_name);
    TimelineNode* next_event(time_t current_time);
    void events_at_time(time_t time) const;

    void print() const;
};

#endif
