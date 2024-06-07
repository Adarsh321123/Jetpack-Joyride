#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <emscripten.h>

static bool mounted = false;

void init_achievements_file(const char *achievements_filename) {
  FILE *achievements_file = fopen(achievements_filename, "w");
  assert(achievements_file != NULL);
  fprintf(achievements_file, "%s\n", "Adarsh");
  fflush(achievements_file);
  int close_result = fclose(achievements_file);  // using int from Adam's example
  assert(close_result == 0);
  fprintf(stderr, "Initialized new achievements file\n");
}

void read_achievements(const char *achievements_filename) {
    FILE *achievements_file = fopen(achievements_filename, "r");
    if (achievements_file == NULL) {
        fprintf(stderr, "Achievements file not found. Creating a new one.\n");
        init_achievements_file(achievements_filename);
        achievements_file = fopen(achievements_filename, "r");
        assert(achievements_file != NULL);
    }
    // TODO: create file if nonexistent
    // TODO: if first time ever playing will need to init the storage
    fprintf(stderr, "File opened for reading\n");
    size_t char_read = 100;
    char *temp_string = malloc(sizeof(char) * (char_read + 1));
    while(fgets(temp_string, char_read, achievements_file)) {
        fprintf(stderr, "%s", temp_string);
    }
    int close_result = fclose(achievements_file);  // using int from Adam's example
    assert(close_result == 0);
    free(temp_string);
}

void write_achievements(const char *achievements_filename) {
    FILE *achievements_file = fopen(achievements_filename, "w");
    assert(achievements_file != NULL);
    fprintf(stderr, "File opened for writing\n");
    fprintf(achievements_file, "Player Name: %s\n", "Rayhan");
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

void sync_from_persistent_storage_and_read() {
    EM_ASM(
        FS.syncfs(true, function (err) {
            assert(!err);
            console.log("Filesystem synchronized from persistent storage for reading.");
            ccall('read_achievements', 'void', ['string'], ['/persistent/achievements.txt']);
        });
    );
}

void sync_from_persistent_storage_and_write() {
    EM_ASM(
        FS.syncfs(true, function (err) {
            assert(!err);
            console.log("Filesystem synchronized from persistent storage for writing.");
            ccall('write_achievements', 'void', ['string'], ['/persistent/achievements.txt']);
            ccall('sync_to_persistent_storage', 'void', []);
        });
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
int main() {
    fprintf(stderr, "Inside persistence.c main()\n");
    // mount the persistent filesystem
    mount_persistent_fs();

    // sync the filesystem from IndexedDB to the in-memory filesystem
    // then, write and sync back to persistent storage
    sync_from_persistent_storage_and_read();
    sync_from_persistent_storage_and_write();

    return 0;
}