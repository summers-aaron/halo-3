#ifndef RENDER_DEPTH_OF_FIELD_TYPES_H
#define RENDER_DEPTH_OF_FIELD_TYPES_H

#include <stddef.h>

struct s_depth_of_field {
  bool enabled;
  bool unknown;
  float amount;
  float near_distance;
  float far_distance;
  float transition;
  float focus_a;
  float focus_b;
  long a;
  long b;
  long c;
  long d;
  float e;
  float focus_distance;
  float g;
  float h;
  float i;
  __forceinline void update() { update_internal(); }
  static void initialize(void);
  static void initialize_for_new_map(void);
  static void dispose(void);
  static void dispose_from_old_map(void);
private:
  void update_internal(void);
};

typedef char depth_of_field_amount_offset[(offsetof(s_depth_of_field, amount) == 4) ? 1 : -1];
typedef char depth_of_field_focus_distance_offset[(offsetof(s_depth_of_field, focus_distance) == 0x30) ? 1 : -1];
typedef char depth_of_field_size[(sizeof(s_depth_of_field) == 0x40) ? 1 : -1];


#endif
