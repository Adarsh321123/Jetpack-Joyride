#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <emscripten.h>
#include <string.h>

#include "list.h"

typedef struct achievement {
    char *name;
    size_t progress;
    size_t target;
    bool unlocked;
} achievement_t;

typedef struct achievements {
    list_t *achievements_list;
} achievements_t;

const size_t INITIAL_ACHIEVEMENTS = 5;
// static bool mounted = false;
const char *ACHIEVEMENTS_FILENAME = "/achievements.txt";
const char *FIRST_ACHIEVEMENT = "Collect 50 Coins|0|50|false";
const char *SECOND_ACHIEVEMENT = "Travel 1000 Meters In A Game|0|1000|false";
const char *THIRD_ACHIEVEMENT = "Dodge 10 Zappers|0|10|false";

size_t achievements_size(achievements_t *achievements) {
    return list_size(achievements->achievements_list);
}

void init_achievements_file(achievements_t *achievements) {
  FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "w");
  assert(achievements_file != NULL);
  fprintf(achievements_file, "%s\n", FIRST_ACHIEVEMENT);
  fprintf(achievements_file, "%s\n", SECOND_ACHIEVEMENT);
  fprintf(achievements_file, "%s\n", THIRD_ACHIEVEMENT);
  fflush(achievements_file);
  int close_result = fclose(achievements_file);  // using int from Adam's example
  assert(close_result == 0);
  fprintf(stderr, "Initialized new achievements file\n");
}

void read_achievements(achievements_t *achievements) {
    FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "r");
    if (achievements_file == NULL) {
        fprintf(stderr, "Achievements file not found. Creating a new one.\n");
        init_achievements_file(achievements);
        achievements_file = fopen(ACHIEVEMENTS_FILENAME, "r");
        assert(achievements_file != NULL);
    }
    fprintf(stderr, "File opened for reading\n");
    size_t char_read = 256;
    char *line = malloc(sizeof(char) * (char_read + 1));
    while(fgets(line, char_read + 1, achievements_file)) {
        fprintf(stderr, "%s", line);
        achievement_t *achievement = malloc(sizeof(achievement_t));
        line[strcspn(line, "\n")] = '\0';
        char *token = strtok(line, "|");
        if (token != NULL) {
            // TODO: need strdup?
            achievement->name = strdup(token);
            assert(achievement->name);
        }
        token = strtok(NULL, "|");
        if (token != NULL) {
            achievement->progress = atoi(token);
        }
        token = strtok(NULL, "|");
        if (token != NULL) {
            achievement->target = atoi(token);
        }
        token = strtok(NULL, "|");
        if (token != NULL) {
            bool unlocked = strcmp(token, "true") == 0;
            achievement->unlocked = unlocked;
        }
        list_add(achievements->achievements_list, achievement);
    }
    fprintf(stderr, "Read all lines\n");
    fprintf(stderr, "%s\n", ((achievement_t *)list_get(achievements->achievements_list, 0))->name);
    fprintf(stderr, "%zu\n", ((achievement_t *)list_get(achievements->achievements_list, 0))->progress);
    fprintf(stderr, "%zu\n", ((achievement_t *)list_get(achievements->achievements_list, 0))->target);
    if (((achievement_t *)list_get(achievements->achievements_list, 0))->unlocked) {
        fprintf(stderr, "true\n");
    } else {
        fprintf(stderr, "false\n");
    }
    int close_result = fclose(achievements_file);  // using int from Adam's example
    assert(close_result == 0);
    free(line);
}

void write_achievements(achievements_t *achievements) {
    FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "w");
    assert(achievements_file != NULL);
    fprintf(stderr, "File opened for writing\n");
    size_t num_achievements = achievements_size(achievements);
    for (size_t i = 0; i < num_achievements; i++) {
        achievement_t *achievement = list_get(achievements->achievements_list, i);
        char *unlocked = NULL;
        if (achievement->unlocked) {
            unlocked = "true";
        } else {
            unlocked = "false";
        }
        fprintf(achievements_file, "%s|%zu|%zu|%s\n",
                achievement->name,
                achievement->progress + 1,
                achievement->target + 1,
                unlocked);
    }
    fflush(achievements_file);
    fprintf(stderr, "Wrote all lines\n");

    int close_result = fclose(achievements_file);
    assert(close_result == 0);
}

// TODO: clean up later such as constants for strings
// TODO: figure out the issue with extra work only if needed
int main() {
    fprintf(stderr, "Inside persistence.c main()\n");

    achievements_t *achievements = malloc(sizeof(achievements_t));
    assert(achievements != NULL);
    achievements->achievements_list = list_init(INITIAL_ACHIEVEMENTS, (free_func_t)list_free);
    fprintf(stderr, "Initialized achievements list\n");

    read_achievements(achievements);
    write_achievements(achievements);

    return 0;
}