#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <emscripten.h>

// TODO: add header for this to replicate achievmeents later

typedef struct achievement {
    char *name;
    size_t progress;
    size_t target;
    bool unlocked;
} achievement_t;

// TODO: add observer and list
typedef struct achievements {
    achievement_t *achievement;
} achievements_t;

const size_t INITIAL_ACHIEVEMENTS = 5;
static bool mounted = false;
const char *ACHIEVEMENTS_FILENAME = "/persistent/achievements.txt";
const char *FIRST_ACHIEVEMENT = "Collect 50 Coins|0|50|false";
const char *SECOND_ACHIEVEMENT = "Travel 1000 meters|0|1000|false";
const char *THIRD_ACHIEVEMENT = "Dodge 10 Zappers|0|10|false";

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
    char *temp_string = malloc(sizeof(char) * (char_read + 1));
    while(fgets(temp_string, char_read, achievements_file)) {
        fprintf(stderr, "%s", temp_string);
    }
    fprintf(stderr, "Read all lines\n");
    int close_result = fclose(achievements_file);  // using int from Adam's example
    assert(close_result == 0);
    free(temp_string);
}

void write_achievements(achievements_t *achievements) {
    FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "w");
    assert(achievements_file != NULL);
    fprintf(stderr, "File opened for writing\n");
    fprintf(achievements_file, "Player Name: %s\n", "Bobby");
    fflush(achievements_file);
    fprintf(stderr, "File written to\n");

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

    // mount the persistent filesystem
    mount_persistent_fs();

    // sync the filesystem from IndexedDB to the in-memory filesystem
    // then, write and sync back to persistent storage
    sync_from_persistent_storage_and_read(achievements);
    sync_from_persistent_storage_and_write(achievements);

    return 0;
}