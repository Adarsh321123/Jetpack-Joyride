#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <emscripten.h>
#include <string.h>

#include "list.h"

// TODO: add header for this to replicate achievmeents later

typedef struct achievement {
    char *name;
    size_t progress;
    size_t target;
    bool unlocked;
} achievement_t;

// TODO: add observer
typedef struct achievements {
    list_t *achievements_list;
} achievements_t;

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
    while(fgets(line, char_read, achievements_file)) {
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
// // TODO: remove console logs
// void sync_from_persistent_storage_and_write() {
//     EM_ASM(
//         FS.syncfs(true, function (err) {
//             assert(!err);
//             console.log("Filesystem synchronized from persistent storage.");
//             ccall('read_achievements', 'void', ['string'], ['/persistent/achievements.txt']);
//             ccall('write_achievements', 'void', ['string'], ['/persistent/achievements.txt']);
//             ccall('sync_to_persistent_storage', 'void', []);
//         });
//     );
// }

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
// TODO: can we use these macros?
// TODO: figure out the issue with extra work only if needed
int main() {
    fprintf(stderr, "Inside persistence.c main()\n");

    achievements_t *achievements = malloc(sizeof(achievements_t));
    assert(achievements != NULL);
    achievements->achievements_list = list_init(INITIAL_ACHIEVEMENTS, (free_func_t)list_free);
    fprintf(stderr, "Initialized achievements list\n");

    // mount the persistent filesystem
    mount_persistent_fs();

    // sync the filesystem from IndexedDB to the in-memory filesystem
    // then, write and sync back to persistent storage
    sync_from_persistent_storage_and_read(achievements);
    sync_from_persistent_storage_and_write(achievements);

    return 0;
}