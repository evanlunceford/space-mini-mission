#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "time_system.h"

// TimelineNode functions
TimelineNode *create_timeline_node(
    char *payload,
    time_t start,
    time_t end,
    // Null = no dependency
    TimelineNode *dependency
) {
    TimelineNode *timeline_node = malloc(sizeof(TimelineNode));

    if (!timeline_node) {
        printf("[ERROR] create_timeline_node: Failed to allocate node for payload '%s'.\n", payload ? payload : "(null)");
        return NULL;
    }

    timeline_node->payload = payload;
    timeline_node->start_time = start;
    timeline_node->end_time = end;
    timeline_node->dependency = dependency;
    timeline_node->isCompleted = false;
    timeline_node->next = NULL;

    printf("[INFO] Created timeline node: '%s' [start=%ld, end=%ld, dependency=%s]\n",
        payload,
        (long)start,
        (long)end,
        dependency ? dependency->payload : "none"
    );

    return timeline_node;
}

bool complete_timeline_node(TimelineNode *node) {
    if (!node) {
        return false;
    }
    // Check if node's dependency is completed
    if ((node->dependency) && !(node->dependency->isCompleted)){
        printf("[WARN] complete_timeline_node: Cannot complete '%s' — dependency '%s' is not yet completed.\n",
            node->payload,
            node->dependency->payload
        );
        return false;
    }

    node->isCompleted = true;
    return true;
}

bool reset_timeline_node(TimelineNode *node){
    if (!node) {
        return false;
    }

    node->isCompleted = false;
    return true;
}

bool timeline_node_active_at(
    TimelineNode *node,
    time_t time
) {
    if (!node) { 
        return false;
    }

    // If the time specified falls within the timeline nodes range
    if (time >= node->start_time && time < node->end_time) {
        return true;
    }

    return false;
}

bool timeline_node_add_dependency(
    TimelineNode *node,
    TimelineNode *dependency
) {
    if (!node || !dependency) {
        return false;
    }

    node->dependency = dependency;
    return true;
}

void destroy_timeline_node(TimelineNode *node){
    if (!node) {
        return;
    }

    free(node);
}


// Timeline functions

//Initializing with NULL head
Timeline *create_timeline(char *name) {
    if (!name) {
        printf("Timeline requires a name.");
        return NULL;
    }

    Timeline *new_timeline = malloc(sizeof(Timeline));

    if (!new_timeline) {
        printf("[ERROR] create_timeline: Failed to allocate timeline '%s'.\n", name);
        return NULL;
    }

    new_timeline->timeline_name = name;
    new_timeline->head = NULL;

    printf("[INFO] Created timeline: '%s'\n", name);

    return new_timeline;
}

bool timeline_insert_node(
    Timeline *timeline,
    TimelineNode *node
) {
    if (!timeline) {
        printf("[ERROR] timeline_insert_node: Timeline not provided.\n");
        return false;
    }

    if (!node) {
        printf("[ERROR] timeline_insert_node: Node not provided.\n");
        return false;
    }

    // If there aren't any nodes
    if (!timeline->head) {
        timeline->head = node;
        printf("[INFO] timeline_insert_node: Inserted '%s' as head of timeline '%s'.\n",
            node->payload, timeline->timeline_name);
        return true;
    }

    // Handles edge case that node happens at the beginning
    if (node->start_time < timeline->head->start_time) {
        node->next = timeline->head;
        timeline->head = node;
        printf("[INFO] timeline_insert_node: Inserted '%s' before head of timeline '%s'.\n",
            node->payload, timeline->timeline_name);
        return true;
    }

    TimelineNode *current = timeline->head;

    // Checks time stamps to see where node falls
    while (current->next && current->next->start_time <= node->start_time) {
        current = current->next;
    }

    node->next = current->next;
    current->next = node;

    printf("[INFO] timeline_insert_node: Inserted '%s' after '%s' in timeline '%s'.\n",
        node->payload, current->payload, timeline->timeline_name);

    return true;

}

bool timeline_remove_node(
    Timeline *timeline,
    TimelineNode *node
) {
    if (!timeline) {
        printf("Timeline not provided.");
        return false;
    }

    if (!node) {
        printf("Node not provided.");
        return false;
    }

    // Edge case: Head node
    if (timeline->head == node) {
        timeline->head = node->next;
        destroy_timeline_node(node);
        return true;
    }

    // Find the node in the timeline
    TimelineNode *current = timeline->head;

    while (current->next && current->next != node) {
        current = current->next;
    }

    if (!current->next) {
        // We didn't find the node in the timeline
        printf("[ERROR] timeline_remove_node: Node '%s' not found in timeline '%s'.\n",
            node->payload, timeline->timeline_name);
        return false;
    }

    // Otherwise we have found the node, which is in current->next
    current->next = node->next;
    destroy_timeline_node(node);

    return true;

}

// Must be the exact time that an event start at
TimelineNode *timeline_find_event_at(
    Timeline *timeline,
    time_t time
) {
    if (!timeline) {
        printf("Timeline not provided.\n");
        return NULL;
    }

    TimelineNode *current = timeline->head;

    while (current) {
        if (current->start_time == time) {
            return current;
        }

        if (current->start_time > time) {
            break;
        }

        current = current->next;
    }

    printf("No timeline node found for time: %ld\n", (long)time);
    return NULL;
}


TimelineNode *timeline_next_event(
    Timeline *timeline,
    time_t current_time
) {
    if (!timeline) {
        printf("Timeline not provided.\n");
        return NULL;
    }

    TimelineNode *current = timeline->head;

    while (current) {
        if (current->start_time > current_time) {
            return current;
        }

        current = current->next;
    }

    printf("No more events.\n");
    return NULL;
}

bool timeline_detect_conflicts(
    Timeline *timeline
) {
    if (!timeline) {
        printf("Timeline not provided.\n");
        return false;
    }

    TimelineNode *current = timeline->head;

    while (current && current->next) {
        if (current->next->start_time < current->end_time) {
            printf("[WARN] timeline_detect_conflicts: Conflict in timeline '%s' — '%s' [ends=%ld] overlaps with '%s' [starts=%ld].\n",
                timeline->timeline_name,
                current->payload,
                (long)current->end_time,
                current->next->payload,
                (long)current->next->start_time
            );
            return true;
        }

        current = current->next;
    }

    printf("[INFO] timeline_detect_conflicts: No conflicts detected in timeline '%s'.\n", timeline->timeline_name);
    return false;
}

void timeline_print(
    Timeline *timeline
) {
    if (!timeline) {
        printf("Timeline not provided.\n");
        return;
    }

    printf("Timeline: %s\n", timeline->timeline_name);

    TimelineNode *current = timeline->head;

    while (current) {
        printf(
            "Event: %s | Start: %ld | End: %ld | Completed: %s\n",
            current->payload,
            (long)current->start_time,
            (long)current->end_time,
            current->isCompleted ? "true" : "false"
        );

        current = current->next;
    }
}

void destroy_timeline(
    Timeline *timeline
) {
    if (!timeline) {
        return;
    }

    TimelineNode *current = timeline->head;

    while (current) {
        TimelineNode *next = current->next;
        destroy_timeline_node(current);
        current = next;
    }

    free(timeline);
}

// Mission functions
Mission *create_mission(char *name) {
    Mission *mission = malloc(sizeof(Mission));

    if (!mission) {
        printf("Failed to allocate mission.\n");
        return NULL;
    }

    mission->mission_name = name;
    mission->timelines = NULL;
    mission->timeline_count = 0;

    printf("[INFO] Created mission: '%s'\n", name);

    return mission;
}

bool mission_add_timeline(
    Mission *mission,
    Timeline *timeline
) {
    if (!mission || !timeline) {
        printf("Mission or timeline not provided.\n");
        return false;
    }

    Timeline *new_array = realloc(
        mission->timelines,
        sizeof(Timeline) * (mission->timeline_count + 1)
    );

    if (!new_array) {
        printf("Failed to allocate timeline array.\n");
        return false;
    }

    mission->timelines = new_array;

    mission->timelines[mission->timeline_count] = *timeline;

    mission->timeline_count++;

    printf("[INFO] mission_add_timeline: Added timeline '%s' to mission '%s' (total timelines: %d).\n",
        timeline->timeline_name, mission->mission_name, mission->timeline_count);

    return true;
}

Timeline *mission_get_timeline(
    Mission *mission,
    char *timeline_name
) {
    if (!mission || !timeline_name) {
        printf("Mission or timeline name not provided.\n");
        return NULL;
    }

    for (int i = 0; i < mission->timeline_count; i++) {
        if (strcmp(mission->timelines[i].timeline_name, timeline_name) == 0) {
            printf("[INFO] mission_get_timeline: Found timeline '%s' in mission '%s'.\n",
                timeline_name, mission->mission_name);
            return &mission->timelines[i];
        }
    }

    printf("[ERROR] mission_get_timeline: Timeline '%s' not found in mission '%s'.\n",
        timeline_name, mission->mission_name);
    return NULL;
}

TimelineNode *mission_next_event(
    Mission *mission,
    time_t current_time
) {
    if (!mission) {
        printf("Mission not provided.\n");
        return NULL;
    }

    TimelineNode *next_event = NULL;

    for (int i = 0; i < mission->timeline_count; i++) {
        TimelineNode *candidate =
            timeline_next_event(&mission->timelines[i], current_time);

        if (!candidate) {
            continue;
        }

        if (!next_event ||
            candidate->start_time < next_event->start_time) {
            next_event = candidate;
        }
    }

    if (next_event) {
        printf("[INFO] mission_next_event: Next event in mission '%s' is '%s' at t=%ld.\n",
            mission->mission_name, next_event->payload, (long)next_event->start_time);
    } else {
        printf("[INFO] mission_next_event: No upcoming events in mission '%s' after t=%ld.\n",
            mission->mission_name, (long)current_time);
    }

    return next_event;
}

void mission_events_at_time(
    Mission *mission,
    time_t time
) {
    if (!mission) {
        printf("Mission not provided.\n");
        return;
    }

    printf("Events at time %ld:\n", (long)time);

    for (int i = 0; i < mission->timeline_count; i++) {
        Timeline *timeline = &mission->timelines[i];
        TimelineNode *node = timeline->head;

        while (node) {
            if (node->start_time == time) {
                printf(
                    "[%s] %s\n",
                    timeline->timeline_name,
                    node->payload
                );
            }

            node = node->next;
        }
    }
}

void destroy_mission(
    Mission *mission
) {
    if (!mission) {
        return;
    }

    // Free each timeline's nodes — do NOT call destroy_timeline() here because
    // the Timeline structs are embedded in the timelines array (not individually
    // heap-allocated), so calling free() on them would be an invalid free.
    for (int i = 0; i < mission->timeline_count; i++) {
        TimelineNode *current = mission->timelines[i].head;
        while (current) {
            TimelineNode *next = current->next;
            destroy_timeline_node(current);
            current = next;
        }
    }

    free(mission->timelines);
    free(mission);
}