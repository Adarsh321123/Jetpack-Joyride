#include "achievement.h"
// TODO: is the include error why emscripten does not work?
#include <emscripten.h>  // TODO: this is also in emscripten.c, is that fine?

// TODO: should we use the observer subject pattern for game over and switching screens?
// TODO: inconsistent tabs
// TODO: go to OH to ask about this pattern
// TODO: figure out how to add and remove achievements if necessary
// TODO: add init file for ach

const size_t INITIAL_ACHIEVEMENTS = 5;
static bool mounted = false;
const char *ACHIEVEMENTS_FILENAME = "/persistent/achievements.txt";
const char *FIRST_ACHIEVEMENT = "Collect 50 Coins|0|50|false";
const char *SECOND_ACHIEVEMENT = "Travel 1000 meters|0|1000|false";
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
  while(fgets(line, sizeof(line), achievements_file)) {
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
    fprintf(achievements_file, "%s|%zu|%zu|%s\n",
            achievement->name,
            achievement->progress,
            achievement->target,
            unlocked);
  }
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

// void sync_from_persistent_storage_and_write(achievements_t *achievements) {
//   EM_ASM(
//     FS.syncfs(true, function (err) {
//       assert(!err);
//       console.log("Filesystem synchronized from persistent storage.");
//       ccall('read_achievements', 'void', ['number'], [$0]);
//       ccall('write_achievements', 'void', ['number'], [$0]);
//       ccall('sync_to_persistent_storage', 'void', []);
//     }), achievements
//   );
// }

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
        FS.mkdir('/persistent');
      }
      FS.mount(IDBFS, {}, '/persistent');
    );
  mounted = true;
  }
}

// TODO: clean up later such as constants for strings
// TODO: what are all of the console logs like the stderr at beginnning of service worker?
void achievements_on_notify(observer_t *observer, event_t event) {
    // fprintf(stderr, "startign the function pointer call\n");
    // achievements_t *achievements = (achievements_t *)observer;
    // size_t num_achievements = achievements_size(achievements);
    // fprintf(stderr, "Number of achievements: %zu\n", num_achievements);
    // // TODO: maybe do logic in sep funcs instead of switch?
    // switch (event) {
    //     case EVENT_COIN_COLLECTED:
    //         fprintf(stderr, "Achievements recieved EVENT_COIN_COLLECTED\n");
    //         for (size_t i = 0; i < num_achievements; i++) {
    //             achievement_t *cur_achievement = list_get(achievements->achievements_list, i);
    //             fprintf(stderr, "Got achievement %zu\n", i);
    //             // TODO: remove magic string, how can we make this scalable?
    //             if (strcmp(cur_achievement->name, "Collect 50 Coins") == 0) {
    //                 // TODO: update encapsulation later with an "edit" or "put" func to avoid this direct change
    //                 // TODO: save instead of casting each time?
    //                 ((achievement_t *)(achievements->achievements_list->elements[i]))->progress++;
    //                 fprintf(stderr, "Updated progress on collecting coins\n");
    //                 if (((achievement_t *)(achievements->achievements_list->elements[i]))->progress >= ((achievement_t *)(achievements->achievements_list->elements[i]))->target) {
    //                     ((achievement_t *)(achievements->achievements_list->elements[i]))->unlocked = true;
    //                     fprintf(stderr, "Achievement Unlocked: %s\n", ((achievement_t *)(achievements->achievements_list->elements[i]))->name);
    //                 }
    //                 break;
    //             }
    //         }
    //         // TODO: maybe write only at end?
    //         sync_from_persistent_storage_and_write(achievements);
    //         break;
    //     // TODO: add other cases
    //     default:
    //         fprintf(stderr, "default on switch for achievements\n");
    //         break;
    // }
}
// TODO: rename this file achievements (plural)
// TODO: PERSISTENCE NOT WORKING ANYMORE, might be related to other logs about restarting service worker
// TODO: is the table index out of bounds issue related to threads?
achievements_t *achievements_init(on_notify_t test_on_notify) {
    achievements_t *achievements = malloc(sizeof(achievements_t));
    assert(achievements != NULL);
    // TODO: do what asset does which is deal with pointer but dereference the result
    // achievements->observer = observer_init(test_on_notify);
    achievements->observer.on_notify = test_on_notify;
    fprintf(stderr, "Achievements initialized at %p, observer at %p\n", (void*)achievements, (void*)(&(achievements->observer)));
    // TODO: maybe change to achievements free, same with subject and observer
    // TODO: is it double free to free these thigns in this file and then also set list_free as freer?
    // achievements->achievements_list = list_init(INITIAL_ACHIEVEMENTS, (free_func_t)list_free);
    // fprintf(stderr, "Initialized achievements list\n");
    // mount_persistent_fs();
    // // sync the filesystem from IndexedDB to the in-memory filesystem
    // // then, write and sync back to persistent storage
    // // TODO: func names too long like this one
    // sync_from_persistent_storage_and_read(achievements);
    // fprintf(stderr, "Read achievements file into the list\n");
    return achievements;
}

void achievements_free(achievements_t *achievements) {
    list_free(achievements->achievements_list);
    free(achievements);
}