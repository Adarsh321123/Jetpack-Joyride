#include "achievement.h"
#include "constants.h"
#include <emscripten.h>

// TODO: inconsistent tabs

size_t achievements_size(achievements_t *achievements) {
    return list_size(achievements->achievements_list);
}

void init_achievements_file() {
  FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "w");
  assert(achievements_file != NULL);
  fprintf(achievements_file, "%s\n", FIRST_ACHIEVEMENT);
  fprintf(achievements_file, "%s\n", SECOND_ACHIEVEMENT);
  fprintf(achievements_file, "%s\n", THIRD_ACHIEVEMENT);
  fflush(achievements_file);
  int close_result = fclose(achievements_file);  // using int from Adam's example
  assert(close_result == 0);
  // fprintf(stderr, "Initialized new achievements file\n");
}

list_t *read_achievements_settings() {
  FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "r");
  if (achievements_file == NULL) {
    // fprintf(stderr, "Achievements file not found. Creating a new one.\n");
    init_achievements_file();
    achievements_file = fopen(ACHIEVEMENTS_FILENAME, "r");
    assert(achievements_file != NULL);
  }
  // fprintf(stderr, "File opened for reading\n");
  size_t LINE_WITH_TERMINATOR = CHAR_TO_READ + 1;
  char *line = malloc(sizeof(char) * LINE_WITH_TERMINATOR);
  list_t *results = list_init(INITIAL_ACHIEVEMENTS, NULL);
  while(fgets(line, LINE_WITH_TERMINATOR, achievements_file)) {
    // fprintf(stderr, "%s", line);
    char *result = malloc(sizeof(char) * LINE_WITH_TERMINATOR);
    line[strcspn(line, NEWLINE)] = '\0';
    char *token = strtok(line, PIPE);
    if (token != NULL) {
      strcpy(result, token);
    }
    strcat(result, SPACE);
    token = strtok(NULL, PIPE);
    if (token != NULL) {
      strcat(result, token);
    }
    strcat(result, SLASH);
    token = strtok(NULL, PIPE);
    if (token != NULL) {
      strcat(result, token);
    }
    // fprintf(stderr, "result: %s\n", result);
    list_add(results, result);
  }
  int close_result = fclose(achievements_file);
  assert(close_result == 0);
  free(line);
  return results;
}

void read_achievements(achievements_t *achievements) {
  FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "r");
  if (achievements_file == NULL) {
    // fprintf(stderr, "Achievements file not found. Creating a new one.\n");
    init_achievements_file();
    achievements_file = fopen(ACHIEVEMENTS_FILENAME, "r");
    assert(achievements_file != NULL);
  }
  // fprintf(stderr, "File opened for reading\n");
  size_t LINE_WITH_TERMINATOR = CHAR_TO_READ + 1;
  char *line = malloc(sizeof(char) * (LINE_WITH_TERMINATOR));
  while(fgets(line, LINE_WITH_TERMINATOR, achievements_file)) {
    // fprintf(stderr, "%s", line);
    achievement_t *achievement = malloc(sizeof(achievement_t));
    line[strcspn(line, NEWLINE)] = '\0';
    char *token = strtok(line, PIPE);
    if (token != NULL) {
      achievement->name = strdup(token);
      assert(achievement->name);
    }
    token = strtok(NULL, PIPE);
    if (token != NULL) {
      achievement->progress = atoi(token);
    }
    token = strtok(NULL, PIPE);
    if (token != NULL) {
      achievement->target = atoi(token);
    }
    token = strtok(NULL, PIPE);
    if (token != NULL) {
      bool unlocked = strcmp(token, TRUE_TEXT) == 0;
      achievement->unlocked = unlocked;
    }
    list_add(achievements->achievements_list, achievement);
  }
  int close_result = fclose(achievements_file);
  assert(close_result == 0);
  free(line);
}

void write_achievements(achievements_t *achievements) {
  FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "w");
  assert(achievements_file != NULL);
  // fprintf(stderr, "File opened for writing\n");
  size_t num_achievements = achievements_size(achievements);
  for (size_t i = 0; i < num_achievements; i++) {
    achievement_t *achievement = list_get(achievements->achievements_list, i);
    char *unlocked = NULL;
    if (achievement->unlocked) {
      unlocked = strdup(TRUE_TEXT);
    } else {
      unlocked = strdup(FALSE_TEXT);
    }
    assert(unlocked != NULL);

    fprintf(achievements_file, "%s|%zu|%zu|%s\n",
            achievement->name,
            achievement->progress,
            achievement->target,
            unlocked);
    free(unlocked);
  }
  fflush(achievements_file);
  // fprintf(stderr, "Wrote all lines\n");

  int close_result = fclose(achievements_file);
  assert(close_result == 0);
}

// TODO: clean up later such as constants for strings
void achievements_on_notify(observer_t *observer, event_t event, void *aux) {
    // fprintf(stderr, "inside achievements_on_notify\n");
    achievements_t *achievements = (achievements_t *)observer;
    size_t num_achievements = achievements_size(achievements);
    // fprintf(stderr, "Number of achievements: %zu\n", num_achievements);
    // TODO: maybe do logic in sep funcs instead of switch?
    switch (event) {
        case EVENT_COIN_COLLECTED:
            // fprintf(stderr, "Achievements recieved EVENT_COIN_COLLECTED\n");
            for (size_t i = 0; i < num_achievements; i++) {
                achievement_t *cur_achievement = list_get(achievements->achievements_list, i);
                // fprintf(stderr, "Got achievement %zu\n", i);
                // fprintf(stderr, "RIGHT BEFORE GET NAME COINS\n");
                if (strcmp(cur_achievement->name, FIRST_ACHIEVEMENT_NAME) == 0) {
                    // TODO: update encapsulation later with an "edit" or "put" func to avoid this direct change
                    // TODO: save instead of casting each time?
                    if (!((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked) {
                      ((achievement_t *)(achievements->achievements_list->elements[i]))->progress++;
                      // fprintf(stderr, "Progress on collecting coins: %zu / 50\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->progress);
                      if (((achievement_t *)(achievements->achievements_list->elements[i]))->progress >=
                          ((achievement_t *)(achievements->achievements_list->elements[i]))->target) {
                          ((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked = true;
                          // fprintf(stderr, "Achievement Unlocked: %s\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->name);
                          ((achievement_t *)(achievements->achievements_list->elements[i]))->progress = ((achievement_t *)(achievements->achievements_list->elements[i]))->target;  // looks better in settings
                      }
                    }
                    break;
                }
            }
            break;
        case EVENT_DISTANCE_TRAVELED:
          // fprintf(stderr, "Achievements recieved EVENT_DISTANCE_TRAVELED\n");
          char *dist_traveled = (char *)aux;  // TODO: fix
          size_t dist_traveled_num = atoi(dist_traveled);
          for (size_t i = 0; i < num_achievements; i++) {
              achievement_t *cur_achievement = list_get(achievements->achievements_list, i);
              // fprintf(stderr, "Got achievement %zu\n", i);
              // fprintf(stderr, "RIGHT BEFORE GET NAME METERS\n");
              if (strcmp(cur_achievement->name, SECOND_ACHIEVEMENT_NAME) == 0) {
                  // TODO: update encapsulation later with an "edit" or "put" func to avoid this direct change
                  // TODO: save instead of casting each time?
                  // TODO: only update progress if not unlocked sicne then easier to show in settings
                  if (!((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked) {
                    ((achievement_t *)(achievements->achievements_list->elements[i]))->progress = dist_traveled_num;
                    // fprintf(stderr, "Progress on distance traveled: %zu / 1000\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->progress);
                    if (((achievement_t *)(achievements->achievements_list->elements[i]))->progress >=
                        ((achievement_t *)(achievements->achievements_list->elements[i]))->target) {
                        ((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked = true;
                        // fprintf(stderr, "Achievement Unlocked: %s\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->name);
                        ((achievement_t *)(achievements->achievements_list->elements[i]))->progress = ((achievement_t *)(achievements->achievements_list->elements[i]))->target;  // looks better in settings
                    }
                  }
                  break;
              }
          }
          break;
        case EVENT_LASERS_AVOIDED:
          // fprintf(stderr, "Achievements recieved EVENT_LASERS_AVOIDED\n");
          for (size_t i = 0; i < num_achievements; i++) {
              achievement_t *cur_achievement = list_get(achievements->achievements_list, i);
              // fprintf(stderr, "Got achievement %zu\n", i);
              // fprintf(stderr, "RIGHT BEFORE GET NAME LASERS\n");
              if (strcmp(cur_achievement->name, THIRD_ACHIEVEMENT_NAME) == 0) {
                  // TODO: update encapsulation later with an "edit" or "put" func to avoid this direct change
                  // TODO: save instead of casting each time?
                  if (!((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked) {
                    ((achievement_t *)(achievements->achievements_list->elements[i]))->progress++;
                    // fprintf(stderr, "Progress on lasers avoided: %zu / 5\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->progress);
                    if (((achievement_t *)(achievements->achievements_list->elements[i]))->progress >=
                        ((achievement_t *)(achievements->achievements_list->elements[i]))->target) {
                        ((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked = true;
                        // fprintf(stderr, "Achievement Unlocked: %s\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->name);
                        ((achievement_t *)(achievements->achievements_list->elements[i]))->progress = ((achievement_t *)(achievements->achievements_list->elements[i]))->target;  // looks better in settings
                    }
                  }
                  break;
              }
          }
          break;
        default:
            // fprintf(stderr, "default on switch for achievements\n");
            break;
    }
}

achievements_t *achievements_init() {
    achievements_t *achievements = malloc(sizeof(achievements_t));
    assert(achievements != NULL);
    achievements->observer.on_notify = achievements_on_notify;
    achievements->observer.freer = achievements_free;
    // fprintf(stderr, "Achievements initialized at %p, observer at %p\n", (void*)achievements, (void*)(&(achievements->observer)));
    achievements->achievements_list = list_init(INITIAL_ACHIEVEMENTS, (free_func_t)free);
    // fprintf(stderr, "Initialized achievements list\n");

    // achievement_t *achievement = malloc(sizeof(achievement_t));
    // achievement->name = "Collect 50 Coins";
    // achievement->progress = 0;
    // achievement->target = 5;
    // achievement->unlocked = false;
    // list_add(achievements->achievements_list, achievement);
    read_achievements(achievements);
    return achievements;
}

// TODO: heap use after free sometimes
void achievements_free(void *observer) {
    // fprintf(stderr, "Inside achievements_free\n");
    achievements_t *achievements = (achievements_t *)observer;
    write_achievements(achievements);
    size_t num_achievements = achievements_size(achievements);
    for (size_t i = 0; i < num_achievements; i++) {
        achievement_t *achievement = list_get(achievements->achievements_list, i);
        if (achievement != NULL && achievement->name != NULL) {
          free(achievement->name);
        }
    }
    list_free(achievements->achievements_list);
    free(achievements);
}