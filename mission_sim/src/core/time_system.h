#ifndef TIME_SYSTEM_H
#define TIME_SYSTEM_H

#include <time.h>
#include <stdbool.h>


// TimelineNode -> Timeline -> Mission
typedef struct TimelineNode {
    char *payload;
    time_t start_time;
    time_t end_time;
    bool isCompleted;

    // If NULL, node has no dependency
    struct TimelineNode *dependency;

    struct TimelineNode *next;
} TimelineNode;

typedef struct {
    char *timeline_name;
    TimelineNode *head;
} Timeline;

typedef struct {
    char *mission_name;
    Timeline *timelines;
    int timeline_count;
} Mission;


// TimelineNode functions
TimelineNode *create_timeline_node(
    char *payload,
    time_t start,
    time_t end,
    // Null = no dependency
    TimelineNode *dependency
);

bool complete_timeline_node(TimelineNode *node);

bool reset_timeline_node(TimelineNode *node);

bool timeline_node_active_at(
    TimelineNode *node,
    time_t time
);

bool timeline_node_add_dependency(
    TimelineNode *node,
    TimelineNode *dependency
);

void destroy_timeline_node(TimelineNode *node);


// Timeline functions
Timeline *create_timeline(char *name);

bool timeline_insert_node(
    Timeline *timeline,
    TimelineNode *node
);

bool timeline_remove_node(
    Timeline *timeline,
    TimelineNode *node
);

TimelineNode *timeline_find_event_at(
    Timeline *timeline,
    time_t time
);

TimelineNode *timeline_next_event(
    Timeline *timeline,
    time_t current_time
);

bool timeline_detect_conflicts(
    Timeline *timeline
);

void timeline_print(
    Timeline *timeline
);

void destroy_timeline(
    Timeline *timeline
);

// Mission functions
Mission *create_mission(char *name);

bool mission_add_timeline(
    Mission *mission,
    Timeline *timeline
);

Timeline *mission_get_timeline(
    Mission *mission,
    char *timeline_name
);

TimelineNode *mission_next_event(
    Mission *mission,
    time_t current_time
);

void mission_events_at_time(
    Mission *mission,
    time_t time
);

void destroy_mission(
    Mission *mission
);

#endif