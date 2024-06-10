#include "laser.h"

struct laser_state {
  double time_laser;
  double time_laser_spawn;
  double time_laser_activate;
  double laser_generation_time;
  bool laser_inactive;
  bool laser_active;
  list_t *laser_inactive_assets;
  list_t *laser_active_assets;
  list_t *laser_centers;
  list_t *laser_spawn_positions;
  laser_spawn_t laser_spawn_bool;
};

