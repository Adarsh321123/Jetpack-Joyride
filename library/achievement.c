#include "achievement.h"
#include "constants.h"
#include <emscripten.h>

struct achievement
{
  char *name;
  size_t progress;
  size_t target;
  bool unlocked;
};

struct achievements
{
  observer_t observer;
  list_t *achievements_list;
};

observer_t *achievements_get_observer(achievements_t *achievements)
{
  return &(achievements->observer);
}

/**
 * Return the size of the achievements.
 * 
 * @param achievements the achievements to get the size of.
 * @return the size of the achievements.
*/
static size_t achievements_size(achievements_t *achievements)
{
  return list_size(achievements->achievements_list);
}

/**
 * Initialize an achievements file.
*/
static void init_achievements_file()
{
  FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "w");
  assert(achievements_file != NULL);
  fprintf(achievements_file, "%s\n", FIRST_ACHIEVEMENT);
  fprintf(achievements_file, "%s\n", SECOND_ACHIEVEMENT);
  fprintf(achievements_file, "%s\n", THIRD_ACHIEVEMENT);
  fflush(achievements_file);
  int close_result = fclose(achievements_file); // using int from Adam's example
  assert(close_result == 0);
}

/**
 * Check if the file exists and, if not, make a new one.
 * 
 * @return the achievements file.
*/
static FILE *check_achievements_file()
{
  FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "r");
  if (achievements_file == NULL)
  {
    init_achievements_file();
    achievements_file = fopen(ACHIEVEMENTS_FILENAME, "r");
    assert(achievements_file != NULL);
  }
  return achievements_file;
}

list_t *read_achievements_settings()
{
  FILE *achievements_file = check_achievements_file();
  size_t LINE_WITH_TERMINATOR = CHAR_TO_READ + 1;
  char *line = malloc(sizeof(char) * LINE_WITH_TERMINATOR);
  assert(line);
  list_t *results = list_init(INITIAL_ACHIEVEMENTS, NULL);
  while (fgets(line, LINE_WITH_TERMINATOR, achievements_file))
  {
    char *result = malloc(sizeof(char) * LINE_WITH_TERMINATOR);
    assert(result);
    line[strcspn(line, NEWLINE)] = '\0';
    char *token = strtok(line, PIPE);
    if (token != NULL)
    {
      strcpy(result, token);
    }
    strcat(result, SPACE);
    token = strtok(NULL, PIPE);
    if (token != NULL)
    {
      strcat(result, token);
    }
    strcat(result, SLASH);
    token = strtok(NULL, PIPE);
    if (token != NULL)
    {
      strcat(result, token);
    }
    list_add(results, result);
  }
  int close_result = fclose(achievements_file);
  assert(close_result == 0);
  free(line);
  return results;
}

/**
 * Read the achievements into an achievement_t for modification.
 * 
 * @param achievements the achievements to read into.
*/
static void read_achievements(achievements_t *achievements)
{
  FILE *achievements_file = check_achievements_file();
  size_t LINE_WITH_TERMINATOR = CHAR_TO_READ + 1;
  char *line = malloc(sizeof(char) * (LINE_WITH_TERMINATOR));
  assert(line);
  while (fgets(line, LINE_WITH_TERMINATOR, achievements_file))
  {
    achievement_t *achievement = malloc(sizeof(achievement_t));
    assert(achievement);
    line[strcspn(line, NEWLINE)] = '\0';
    char *token = strtok(line, PIPE);
    if (token != NULL)
    {
      achievement->name = strdup(token);
      assert(achievement->name);
    }
    token = strtok(NULL, PIPE);
    if (token != NULL)
    {
      achievement->progress = atoi(token);
    }
    token = strtok(NULL, PIPE);
    if (token != NULL)
    {
      achievement->target = atoi(token);
    }
    token = strtok(NULL, PIPE);
    if (token != NULL)
    {
      bool unlocked = strcmp(token, TRUE_TEXT) == 0;
      achievement->unlocked = unlocked;
    }
    list_add(achievements->achievements_list, achievement);
  }
  int close_result = fclose(achievements_file);
  assert(close_result == 0);
  free(line);
}

/**
 * Write the updated achievements to the file.
 * 
 * @param achievements the achievements to write.
*/
static void write_achievements(achievements_t *achievements)
{
  FILE *achievements_file = fopen(ACHIEVEMENTS_FILENAME, "w");
  assert(achievements_file != NULL);
  size_t num_achievements = achievements_size(achievements);
  for (size_t i = 0; i < num_achievements; i++)
  {
    achievement_t *achievement = list_get(achievements->achievements_list, i);
    char *unlocked = NULL;
    if (achievement->unlocked)
    {
      unlocked = strdup(TRUE_TEXT);
    }
    else
    {
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
  int close_result = fclose(achievements_file);
  assert(close_result == 0);
}

/**
 * Handle the coin collected event and update progress.
 * 
 * @param achievements the achievements to update.
*/
static void handle_coin_collected(achievements_t *achievements)
{
  size_t num_achievements = achievements_size(achievements);
  for (size_t i = 0; i < num_achievements; i++)
  {
    achievement_t *cur_achievement = list_get(achievements->achievements_list, i);
    if (strcmp(cur_achievement->name, FIRST_ACHIEVEMENT_NAME) == 0)
    {
      if (!cur_achievement->unlocked)
      {
        cur_achievement->progress++;
        if (cur_achievement->progress >= cur_achievement->target)
        {
          cur_achievement->unlocked = true;
          cur_achievement->progress = cur_achievement->target; // looks better in settings
        }
      }
      break;
    }
  }
}

/**
 * Handle the distance traveled event and update progress.
 * 
 * @param achievements the achievements to update.
 * @param aux the distance traveled.
*/
static void handle_distance_traveled(achievements_t *achievements, void *aux)
{
  size_t num_achievements = achievements_size(achievements);
  char *dist_traveled = (char *)aux;
  size_t dist_traveled_num = atoi(dist_traveled);
  for (size_t i = 0; i < num_achievements; i++)
  {
    achievement_t *cur_achievement = list_get(achievements->achievements_list, i);
    if (strcmp(cur_achievement->name, SECOND_ACHIEVEMENT_NAME) == 0)
    {
      if (!cur_achievement->unlocked)
      {
        cur_achievement->progress = dist_traveled_num;
        if (cur_achievement->progress >= cur_achievement->target)
        {
          cur_achievement->unlocked = true;
          cur_achievement->progress = cur_achievement->target; // looks better in settings
        }
      }
      break;
    }
  }
}

/**
 * Handle the lasers avoided event and update progress.
 * 
 * @param achievements the achievements to update.
*/
static void handle_lasers_avoided(achievements_t *achievements)
{
  size_t num_achievements = achievements_size(achievements);
  for (size_t i = 0; i < num_achievements; i++)
  {
    achievement_t *cur_achievement = list_get(achievements->achievements_list, i);
    if (strcmp(cur_achievement->name, THIRD_ACHIEVEMENT_NAME) == 0)
    {
      if (!cur_achievement->unlocked)
      {
        cur_achievement->progress++;
        if (cur_achievement->progress >= cur_achievement->target)
        {
          cur_achievement->unlocked = true;
          cur_achievement->progress = cur_achievement->target; // looks better in settings
        }
      }
      break;
    }
  }
}

void achievements_on_notify(observer_t *observer, event_t event, void *aux)
{
  achievements_t *achievements = (achievements_t *)observer;
  switch (event)
  {
    case EVENT_COIN_COLLECTED:
      handle_coin_collected(achievements);
      break;
    case EVENT_DISTANCE_TRAVELED:
      handle_distance_traveled(achievements, aux);
      break;
    case EVENT_LASERS_AVOIDED:
      handle_lasers_avoided(achievements);
      break;
    default:
      break;
  }
}

achievements_t *achievements_init()
{
  achievements_t *achievements = malloc(sizeof(achievements_t));
  assert(achievements != NULL);
  achievements->observer.on_notify = achievements_on_notify;
  achievements->observer.freer = achievements_free;
  achievements->achievements_list = list_init(INITIAL_ACHIEVEMENTS, (free_func_t)free);
  read_achievements(achievements);
  return achievements;
}

void achievements_free(void *observer)
{
  achievements_t *achievements = (achievements_t *)observer;
  write_achievements(achievements);
  size_t num_achievements = achievements_size(achievements);
  for (size_t i = 0; i < num_achievements; i++)
  {
    achievement_t *achievement = list_get(achievements->achievements_list, i);
    if (achievement != NULL && achievement->name != NULL)
    {
      free(achievement->name);
    }
  }
  list_free(achievements->achievements_list);
  free(achievements);
}