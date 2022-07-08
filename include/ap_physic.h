/**
 * @file ap_physic.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief physic functions definition
 *
 */

#ifndef AP_PHYSIC_H
#define AP_PHYSIC_H

#include <stdbool.h>

/**
 * @brief PI definition: 3.14
 *
 */
#ifndef AP_PI
#define AP_PI 3.14f
#endif

/**
 * @brief Gravity definition: 15.0f
 */
#ifndef AP_G
#define AP_G 15.0f
#endif

/**
 * @brief creature moving direction
 */
typedef enum {
        AP_DIRECTION_UNKNOWN = 0,
        AP_DIRECTION_FORWARD,
        AP_DIRECTION_BACKWORD,
        AP_DIRECTION_LEFT,
        AP_DIRECTION_RIGHT,
        AP_DIRECTION_UP,
        AP_DIRECTION_DOWN,
        AP_DIRECTION_LENGTH
} AP_Physic_directions;

/**
 * Creature modes
 * @todo still wip
 */
typedef enum {
        AP_CREATURE_MODE_UNKNOW = 0,
        AP_CREATURE_MODE_SURVIVAL,
        AP_CREATURE_MODE_CREATIVE,
        AP_CREATURE_MODE_SPECTATOR,
        AP_CREATURE_MODE_LENGTH
} AP_Creature_modes;

/**
 * barrier types
 */
typedef enum {
        AP_BARRIER_TYPE_UNKNOWN,
        /** barrier type box */
        AP_BARRIER_TYPE_BOX,
        /** barrier type ball*/
        AP_BARRIER_TYPE_BALL,
        AP_BARRIER_TYPE_LENGTH
} AP_Barrier_types;

/**
 * Definition of 3D Collision detection items
 */

/**
 * The movement status of one item
 */
struct AP_PMovement {
        /** current acceleration */
        float acceleration[3];
        /** current speed */
        float speed[3];
};

/**
 * Box struct object definition,
 * used for collition detection
 */
struct AP_PBox {
        /**
         * The position is the center of the box
         * (x / 2, y / 2, z / 2)
         */
        float pos[3];
        /** box size */
        float size[3];
};

/**
 * Ball struct object definition,
 * used for collition detection
 */
struct AP_PBall {
        /** pos is the center of the ball */
        float pos[3];
        /** radius */
        float r;
};

/**
 * Barrier struct object definition,
 * barrier is invisible, only used for collision detection
 */
struct AP_PBarrier {
        /** barrier id */
        int id;
        /** barrier type */
        int type;
        struct AP_PBox box;
        struct AP_PBall ball;
};

/**
 * Creature, such as human, animals etc...
 * Each creature have eyes,
 * and use AP_PBox for collistion detection calculation
 */
struct AP_PCreature {
        /** creature id */
        unsigned int id;
        /** camera id */
        unsigned int camera_id;
        /** collistion box */
        struct AP_PBox box;
        /** used for calculate moving speed... */
        struct AP_PMovement move;
        /** creature is standing or not */
        bool floating;
        /**
         * camera offset:
         * xyz = ( box.length / 2, eyes_height, box.width / 2)
         *
         * used for set the position of the eyes (camera)
         */
        float camera_offset[3];
        // float eyes_height;      // used to set camera height
        /** speed value for walk/fly (process camera movement) */
        float move_speed;
        /** speed value for jumping */
        float jump_speed;
        /** @see AP_Creature_modes */
        int mode;
};

int ap_physic_init();
int ap_physic_free_all();
int ap_physic_free_creature(int id);
int ap_physic_free_barrier(int id);

/**
 * @brief generate one creature struct object
 *
 * @param id [out] creature id
 * @return int AP_Error_Types
 */
int ap_physic_generate_creature(unsigned int *id, float size[3]);

int ap_creature_set_camera_offset(float offset[3]);

/**
 * @brief Get the pointer points to struct AP_PCreature
 *
 * @param id  [in] id
 * @param ptr [out] pointer points to the pointer of struct AP_PCreature
 * @return int AP_Error_Types
 */
int ap_physic_get_creature_ptr(unsigned int id, struct AP_PCreature **ptr);

/**
 * @brief Update all generated creature status
 *
 * @return int
 */
int ap_physic_update_creature();

int ap_creature_use(unsigned int id);

int ap_creature_set_pos(float pos[3]);

int ap_creature_process_move(int direction, float speed);

int ap_creature_jump();

/**
 * @brief generate a barrier with its type
 *
 * @param id [out] pointer points to id
 * @param type AP_Barrier_types
 * @return int AP_Error_Types
 */
int ap_physic_generate_barrier(unsigned int *id, int type);

int ap_barrier_get_ptr(unsigned int id, struct AP_PBarrier **ptr);

int ap_barrier_remove(unsigned int id);

/**
 * @brief Collision detection between two cube boc
 *
 * @param box1
 * @param box2
 * @return true two box crossed
 * @return false two box not crossed
 */
bool ap_box_box_collision_test(
        struct AP_PBox *box1,
        struct AP_PBox *box2);

/**
 * @see ap_box_box_collision_test
 */
bool ap_box_ball_collistion_test(
        struct AP_PBox *box,
        struct AP_PBall *ball);

/**
 * @brief Collision detection between two box
 * If the movable box and barrial box is crossed (hit),
 * move the moveable box to the closest side (face) of the barriar.
 *
 * @param barrial_box barriar box
 * @param movable_box the movable box
 * @param on_top [out] the movable box is standing on the top of the barriar
 *                     or not, can be NULL
 * @return int AP_Error_Types
 */
int ap_box_box_collision_move(
        struct AP_PBox *barrial_box,
        struct AP_PBox *movable_box,
        bool *on_top
);

int ap_box_ball_collision_move(
        struct AP_PBox *barrial_box,
        struct AP_PBox *movable_ball
);

int ap_ball_box_collision_move(
        struct AP_PBall *barrial_ball,
        struct AP_PBox *movable_box
);

int ap_ball_ball_collision_move(
        struct AP_PBall *barrial_ball,
        struct AP_PBall *movable_ball
);

/**
 * @brief Set the central position of the barrier
 *
 * @param id barrier ID
 * @param pos vec3
 * @return int AP_Error_Types
 */
int ap_barrier_set_pos(unsigned int id, float pos[3]);

/**
 * @see ap_barrier_set_pos
 */
int ap_barrier_set_size(unsigned int id, float size[3]);

#endif // AP_PHYSIC_H
