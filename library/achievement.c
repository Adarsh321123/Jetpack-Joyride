#include "achievement.h"
#include <emscripten.h>

// TODO: inconsistent tabs
// TODO: figure out how to add and remove achievements if necessary
// TODO: occasional weird output in unicode other lang?
// TODO: keep testing and make sure no async write and read problems
// TODO: if possible, make this async stuff sync so we can write right before end and not interfere with reading

const size_t INITIAL_ACHIEVEMENTS = 3;
static bool mounted = false;  // TODO: bad practice
const char *ACHIEVEMENTS_FILENAME = "/persistent/achievements.txt";
const char *FIRST_ACHIEVEMENT = "Collect 50 Coins|0|50|false";
const char *SECOND_ACHIEVEMENT = "Travel 1000 Meters|0|1000|false";
const char *THIRD_ACHIEVEMENT = "Avoid 5 Lasers|0|5|false";

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
  int close_result = fclose(achievements_file);
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
    // TODO: got this once:
    /**
     * Uncaught RuntimeError: memory access out of bounds
    at game.wasm.memchr (memchr.c:17:40)
    at game.wasm.strnlen (strnlen.c:5:18)
    at game.wasm.printf_core (vfprintf.c:654:12)
    at game.wasm.__vfprintf_internal (vfprintf.c:746:13)
    at game.wasm.vfiprintf (vfprintf.c:769:9)
    at game.wasm.fiprintf (fprintf.c:21:8)
    at game.wasm.write_achievements (achievement.c:95:5)
    at game.js:865:12
    at ccall (game.js:9639:17)
    at game.js:1169:144
     */

    fprintf(achievements_file, "%s|%zu|%zu|%s\n",
            achievement->name,
            achievement->progress,
            achievement->target,
            unlocked);
  }
  fflush(achievements_file);
  fprintf(stderr, "Wrote all lines\n");

  int close_result = fclose(achievements_file);
  assert(close_result == 0);
}

void sync_to_persistent_storage() {
  EM_ASM(
    FS.syncfs(function (err) {
      assert(!err);
      console.log("Filesystem synchronized to persistent storage.");
    });
  );
}

// TODO: remove console logs
void sync_from_persistent_storage_and_read(achievements_t *achievements) {
    EM_ASM(
        FS.syncfs(true, function (err) {
            assert(!err);
            console.log("Filesystem synchronized from persistent storage for reading.");
            ccall('read_achievements', 'void', ['number'], [$0]);
        }), achievements
    );
}

void sync_from_persistent_storage_and_write(achievements_t *achievements) {
    EM_ASM(
        FS.syncfs(true, function (err) {
            assert(!err);
            console.log("Filesystem synchronized from persistent storage for writing.");
            ccall('write_achievements', 'void', ['number'], [$0]);
            ccall('sync_to_persistent_storage', 'void', []);
        }), achievements
    );
}

void mount_persistent_fs() {
  if (!mounted) {
    EM_ASM(
      if (!FS.analyzePath('/persistent').exists) {
        console.log("/persistent does not exist");
        FS.mkdir('/persistent');
      }
      FS.mount(IDBFS, {}, '/persistent');
    );
    mounted = true;
  }
}

// TODO: clean up later such as constants for strings
// TODO: what are all of the console logs like the stderr at beginnning of service worker?
void achievements_on_notify(observer_t *observer, event_t event, void *aux) {
    fprintf(stderr, "inside achievements_on_notify\n");
    achievements_t *achievements = (achievements_t *)observer;
    size_t num_achievements = achievements_size(achievements);
    fprintf(stderr, "Number of achievements: %zu\n", num_achievements);
    // TODO: maybe do logic in sep funcs instead of switch?
    switch (event) {
        case EVENT_COIN_COLLECTED:
            fprintf(stderr, "Achievements recieved EVENT_COIN_COLLECTED\n");
            for (size_t i = 0; i < num_achievements; i++) {
                achievement_t *cur_achievement = list_get(achievements->achievements_list, i);
                fprintf(stderr, "Got achievement %zu\n", i);
                // TODO: remove magic string, how can we make this scalable?
                if (strcmp(cur_achievement->name, "Collect 50 Coins") == 0) {
                    // TODO: update encapsulation later with an "edit" or "put" func to avoid this direct change
                    // TODO: save instead of casting each time?
                    ((achievement_t *)(achievements->achievements_list->elements[i]))->progress++;
                    fprintf(stderr, "Progress on collecting coins: %zu / 50\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->progress);
                    if (!((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked &&
                        ((achievement_t *)(achievements->achievements_list->elements[i]))->progress >=
                        ((achievement_t *)(achievements->achievements_list->elements[i]))->target) {
                        ((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked = true;
                        fprintf(stderr, "Achievement Unlocked: %s\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->name);
                    }
                    break;
                }
            }
            break;
        case EVENT_DISTANCE_TRAVELED:
          fprintf(stderr, "Achievements recieved EVENT_DISTANCE_TRAVELED\n");
          char *dist_traveled = (char *)aux;
          size_t dist_traveled_num = atoi(dist_traveled);
          for (size_t i = 0; i < num_achievements; i++) {
              achievement_t *cur_achievement = list_get(achievements->achievements_list, i);
              fprintf(stderr, "Got achievement %zu\n", i);
              // TODO: remove magic string, how can we make this scalable?
              if (strcmp(cur_achievement->name, "Travel 1000 Meters") == 0) {
                  // TODO: update encapsulation later with an "edit" or "put" func to avoid this direct change
                  // TODO: save instead of casting each time?
                  ((achievement_t *)(achievements->achievements_list->elements[i]))->progress += dist_traveled_num;
                  fprintf(stderr, "Progress on distance traveled: %zu / 1000\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->progress);
                  if (!((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked &&
                      ((achievement_t *)(achievements->achievements_list->elements[i]))->progress >=
                      ((achievement_t *)(achievements->achievements_list->elements[i]))->target) {
                      ((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked = true;
                      fprintf(stderr, "Achievement Unlocked: %s\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->name);
                  }
                  break;
              }
          }
          break;
        case EVENT_LASERS_AVOIDED:
          fprintf(stderr, "Achievements recieved EVENT_LASERS_AVOIDED\n");
          for (size_t i = 0; i < num_achievements; i++) {
              achievement_t *cur_achievement = list_get(achievements->achievements_list, i);
              fprintf(stderr, "Got achievement %zu\n", i);
              // TODO: remove magic string, how can we make this scalable?
              if (strcmp(cur_achievement->name, "Avoid 5 Lasers") == 0) {
                  // TODO: update encapsulation later with an "edit" or "put" func to avoid this direct change
                  // TODO: save instead of casting each time?
                  ((achievement_t *)(achievements->achievements_list->elements[i]))->progress++;
                  fprintf(stderr, "Progress on lasers avoided: %zu / 5\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->progress);
                  if (!((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked &&
                      ((achievement_t *)(achievements->achievements_list->elements[i]))->progress >=
                      ((achievement_t *)(achievements->achievements_list->elements[i]))->target) {
                      ((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked = true;
                      fprintf(stderr, "Achievement Unlocked: %s\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->name);
                  }
                  break;
              }
          }
          break;
        default:
            fprintf(stderr, "default on switch for achievements\n");
            break;
    }
    // write and sync back to persistent storage
    // TODO: since this is async, is it possible that the last one is not recorded before free, resulting in heap use after free?
    sync_from_persistent_storage_and_write(achievements);
    fprintf(stderr, "wrote and synced\n");
}
// TODO: rename this file achievements (plural)
// TODO: laggy
achievements_t *achievements_init() {
    achievements_t *achievements = malloc(sizeof(achievements_t));
    assert(achievements != NULL);
    achievements->observer.on_notify = achievements_on_notify;
    achievements->observer.freer = achievements_free;
    fprintf(stderr, "Achievements initialized at %p, observer at %p\n", (void*)achievements, (void*)(&(achievements->observer)));
    achievements->achievements_list = list_init(INITIAL_ACHIEVEMENTS, (free_func_t)free);
    fprintf(stderr, "Initialized achievements list\n");

    // TODO: remove after testing
    // achievement_t *achievement = malloc(sizeof(achievement_t));
    // achievement->name = "Collect 50 Coins";
    // achievement->progress = 0;
    // achievement->target = 5;
    // achievement->unlocked = false;
    // list_add(achievements->achievements_list, achievement);

    mount_persistent_fs();
    fprintf(stderr, "Mounted persistent file storage\n");
    // sync the filesystem from IndexedDB to the in-memory filesystem
    // TODO: func names too long like this one
    sync_from_persistent_storage_and_read(achievements);
    fprintf(stderr, "synced and read\n");
    fprintf(stderr, "Read achievements file into the list\n");
    return achievements;
}

void achievements_free(void *observer) {
    achievements_t *achievements = (achievements_t *)observer;
    fprintf(stderr, "Inside achievements_free\n");
    list_free(achievements->achievements_list);
    free(achievements);
}