#include "ap_physic.h"
#include "ap_utils.h"
#include "ap_cvector.h"
#include "ap_camera.h"
#include "ap_render.h"
#include "ap_math.h"

#include  "cglm/cglm.h"

/**
 * @brief Update specific creature by its pointer
 *
 * @param ptr pointer points to creature
 * @return int AP_Types
 */
static int ap_physic_update_creature_ptr(struct AP_PCreature *ptr);
static int ap_creature_process_barrier();
static int ap_creature_process_barrier_ptr(
        struct AP_PCreature *creature,
        struct AP_PBarrier *barrier,
        bool *on_top);

struct AP_Vector creature_vector = { 0, 0, 0, 0 };
struct AP_Vector barrier_vector  = { 0, 0, 0, 0 };
struct AP_PCreature *creature_using = NULL;

int ap_physic_init()
{
        if (creature_vector.data && barrier_vector.data) {
                return 0;
        }
        ap_vector_init(&creature_vector, AP_VECTOR_PCREATURE);
        ap_vector_init(&barrier_vector,  AP_VECTOR_PBARRIER);

        return 0;
}

int ap_physic_generate_creature(unsigned int *id, float size[3])
{
        if (!id) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        struct AP_PCreature creature;
        memset(&creature, 0, sizeof(struct AP_PCreature));
        int ret = 0;
        creature.id = creature_vector.length + 1;
        creature.move_speed = 5.0f;     // speed of camera movement
        creature.jump_speed = 5.5f;
        creature.move.acceleration[1] = -AP_G;  // gravaty
        memcpy(creature.box.size, size, VEC3_SIZE);
        unsigned int cam_id = 0;
        ret = ap_camera_generate(&cam_id);
        if (ret != 0) {
                return ret;
        }
        creature.camera_id = !ret ? cam_id : 0;
        ret = ap_vector_push_back(&creature_vector, (char*) &creature);
        *id = (!ret) ? creature.id : 0;

        return ret;
}

int ap_creature_set_pos(float pos[3])
{
        if (!creature_using) {
                return AP_ERROR_INIT_FAILED;
        }
        if (!pos) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        float center_pos[3] = {
                pos[0] + creature_using->box.size[0] / 2,
                pos[1] + creature_using->box.size[1] / 2,
                pos[2] + creature_using->box.size[2] / 2,
        };
        memcpy(creature_using->box.pos, center_pos, VEC3_SIZE);

        return 0;
}

int ap_creature_set_camera_offset(float offset[3])
{
        if (!creature_using) {
                return AP_ERROR_INIT_FAILED;
        }
        memcpy(creature_using->camera_offset, offset, VEC3_SIZE);
        return 0;
}

int ap_physic_get_creature_ptr(
        unsigned int id, struct AP_PCreature **ptr)
{
        if (!ptr) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        if (!creature_vector.data) {
                return AP_ERROR_INIT_FAILED;
        }

        *ptr = NULL;
        struct AP_PCreature *data = (struct AP_PCreature*) creature_vector.data;
        for (int i = 0; i < creature_vector.length; ++i) {
                if (data[i].id == id) {
                        *ptr = data + i;
                        return 0;
                }
        }

        return 0;
}

int ap_physic_update_creature_id(unsigned int id)
{
        struct AP_PCreature *ptr = NULL;
        ap_physic_get_creature_ptr(id, &ptr);
        if (!ptr) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        ap_physic_update_creature_ptr(ptr);

        return 0;
}

int ap_physic_update_creature()
{
        for (int i = 0; i < creature_vector.length; ++i) {
                struct AP_PCreature *data
                        = (struct AP_PCreature*) creature_vector.data;
                ap_physic_update_creature_ptr(data + i);
        }

        return 0;
}

int ap_physic_update_creature_ptr(struct AP_PCreature *ptr)
{
        if (!ptr) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        int ret = 0;
        struct AP_Camera *old_cam = ap_get_current_camera();
        ret = ap_camera_use(ptr->camera_id);
        if (ret) {
                LOGE("ap_camera_use failed");
        }

        vec3 cam_pos = {
                ptr->box.pos[0] + ptr->camera_offset[0], // x
                ptr->box.pos[1] + ptr->camera_offset[1], // y (height)
                ptr->box.pos[2] + ptr->camera_offset[2], // z
        };
        ap_camera_set_position(cam_pos[0], cam_pos[1], cam_pos[2]);
        ap_camera_use((old_cam) ? old_cam->id : 0);

        // vt = v0 + a * delta_t;
        // xt = v0 * dt + 0.5 * a * t^2
        // calculate movements
        float dt = 0.0f;
        ap_render_get_dt(&dt);
        if (!ptr->floating) {
                ptr->move.speed[1] = 0.0f;
        } else {
                ptr->move.speed[1] += ptr->move.acceleration[1] * dt;
                ptr->box.pos[1] += ptr->move.speed[1] * dt;
        }


        ap_creature_process_barrier();

        if (creature_using->box.pos[1] < -64.0f) {
                creature_using->move.speed[1] = 0.0f;
                creature_using->box.pos[0] = creature_using->box.pos[2] = 0.0f;
                creature_using->box.pos[1] = 5.0f;
                LOGD("creature fall out of the world");
        }

        return 0;
}

int ap_creature_use(unsigned int id)
{
        if (id == 0) {
                creature_using = NULL;
                return 0;
        }

        struct AP_PCreature *ptr = NULL;
        ap_physic_get_creature_ptr(id, &ptr);
        if (!ptr) {
                LOGW("failed to use creature");
                return 0;
        }

        creature_using = ptr;
        ap_camera_use(ptr->camera_id);

        return 0;
}

int ap_creature_process_move(int direction, float speed_up)
{
        if (!creature_using) {
                return AP_ERROR_SUCCESS;
        }

        struct AP_Camera *old_cam = ap_get_current_camera();

        float dt = 0.0f;
        ap_render_get_dt(&dt);
        float velocity = creature_using->move_speed * dt * speed_up;
        vec3 temp = { 0.0f, 0.0f, 0.0f };
        // "MineCraft like" camera
        vec3 front = { 0.0f, 0.0f, 0.0f };
        ap_camera_get_front(front);
        vec3 x0z = { front[0], 0.0f, front[2] };
        glm_vec3_normalize(x0z);
        vec3 up = { 0.0f, 1.0f, 0.0f };
        switch (direction)
        {
        case AP_DIRECTION_FORWARD:
        {
                glm_vec3_scale(x0z, velocity, temp);
                glm_vec3_add(
                        creature_using->box.pos,
                        temp,
                        creature_using->box.pos
                );
                break;
        }
        case AP_DIRECTION_BACKWORD:
        {
                glm_vec3_scale(x0z, velocity, temp);
                glm_vec3_sub(
                        creature_using->box.pos,
                        temp,
                        creature_using->box.pos
                );
                break;
        }
        case AP_DIRECTION_LEFT:
        {
                glm_vec3_cross(x0z, up, temp);
                glm_vec3_normalize(temp);
                glm_vec3_scale(temp, velocity, temp);
                glm_vec3_sub(
                        creature_using->box.pos,
                        temp,
                        creature_using->box.pos
                );
                break;
        }
        case AP_DIRECTION_RIGHT:
        {
                glm_vec3_cross(x0z, up, temp);
                glm_vec3_normalize(temp);
                glm_vec3_scale(temp, velocity, temp);
                glm_vec3_add(
                        creature_using->box.pos,
                        temp,
                        creature_using->box.pos
                );
                break;
        }
        case AP_DIRECTION_UP:
        {
                glm_vec3_scale(up, velocity, temp);
                glm_vec3_add(
                        creature_using->box.pos,
                        temp,
                        creature_using->box.pos
                );
                break;
        }
        case AP_DIRECTION_DOWN:
        {
                glm_vec3_scale(up, velocity, temp);
                glm_vec3_sub(
                        creature_using->box.pos,
                        temp,
                        creature_using->box.pos
                );
                break;
        }
        default:
                break;
        }

        ap_camera_use(old_cam ? old_cam->id : 0);
        return 0;
}

int ap_creature_process_view()
{
        return 0;
}

int ap_creature_jump()
{
        if (!creature_using) {
                return AP_ERROR_SUCCESS;
        }

        if (!creature_using->floating) {
                LOGD("start floating");
                creature_using->floating = true;
        } else {
                return 0;
        }

        creature_using->move.speed[1] = creature_using->jump_speed;

        return 0;
}

int ap_physic_generate_barrier(unsigned int *id, int type)
{
        if (!id || type >= AP_BARRIER_TYPE_LENGTH
                || type <= AP_BARRIER_TYPE_UNKNOWN)
        {
                return AP_ERROR_INVALID_PARAMETER;
        }

        *id = 0;
        struct AP_PBarrier barrier;
        memset(&barrier, 0, sizeof(struct AP_PBarrier));

        barrier.type = type;
        barrier.id = barrier_vector.length + 1;
        int ret = ap_vector_push_back(&barrier_vector, (char*) &barrier);
        if (ret == 0) {
                *id = barrier.id;
        }

        return 0;
}

int ap_barrier_get_ptr(unsigned int id, struct AP_PBarrier **ptr)
{
        if (!ptr) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        if (id <= 0 || id > barrier_vector.length) {
                LOGE("ap_barrier_get_ptr: invalid id");
                return AP_ERROR_INVALID_PARAMETER;
        }

        *ptr = NULL;
        struct AP_PBarrier *data = (struct AP_PBarrier*) barrier_vector.data;
        for (int i = 0; i < barrier_vector.length; ++i) {
                if (data[i].id == id) {
                        *ptr = data + i;
                        return 0;
                }
        }
        return 0;
}

int ap_barrier_remove(unsigned int id)
{
        struct AP_PBarrier *start = NULL;
        ap_barrier_get_ptr(id, &start);
        if (start == NULL) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        struct AP_PBarrier *end = start + 1;
        ap_vector_remove_data(
                &barrier_vector,
                (char*) start, (char*) end,
                sizeof(struct AP_PBarrier)
        );

        return 0;
}

bool ap_box_box_collision_test(
        struct AP_PBox *box1,
        struct AP_PBox *box2)
{
        if (!box1 || !box2) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        float min_dis[3] = { 0.0f };
        float dis[3] = { 0.0f };
        for (int i = 0; i < 3; ++i) {
                min_dis[i] = (box1->size[i] + box2->size[i]) / 2.0f;
                dis[i] = ap_absf(box1->pos[i] - box2->pos[i]);
        }

        int count = 0;
        for (int i = 0; i < 3; ++i) {
                if (min_dis[i] < dis[i]) {
                        ++count;
                }
        }

        return (count == 3);
}

bool ap_box_ball_collistion_test(
        struct AP_PBox *box,
        struct AP_PBall *ball)
{
        if (!box || !ball) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        float min_dis[3] = { 0.0f };
        float dis[3] = { 0.0f };
        for (int i = 0; i < 3; ++i) {
                min_dis[i] = box->size[i] / 2.0f + ball->r;
                dis[i] = ap_absf(box->pos[i] - ball->pos[i]);
        }
        int count = 0;
        for (int i = 0; i < 3; ++i) {
                if (min_dis[i] < dis[i]) {
                        ++count;
                }
        }
        return (count == 3);
}

bool ap_ball_ball_collision_text(
        struct AP_PBall *ball1, struct AP_PBall *ball2)
{
        if (!ball1 || !ball2) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        float min_dis = ball1->r + ball2->r;
        float dis = ap_distance(ball1->pos, ball2->pos);
        return (dis <= min_dis);
}

static int ap_creature_process_barrier_ptr(
        struct AP_PCreature *creature,
        struct AP_PBarrier *barrier,
        bool *on_top)
{
        if (!creature || !barrier) {
                return 0;
        }

        switch (barrier->type)
        {
        case AP_BARRIER_TYPE_BOX:
        {
                ap_box_box_collision_move(
                        &barrier->box, &creature->box, on_top);
                break;
        }
        case AP_BARRIER_TYPE_BALL:
        {
                // TODO:
                break;
        }
        default:
                break;
        }

        return 0;
}

static int ap_creature_process_barrier()
{
        if (!creature_using) {
                return 0;
        }

        struct AP_PBarrier *data = (struct AP_PBarrier*) barrier_vector.data;
        bool is_standing = false;
        for (int i = 0; i < barrier_vector.length; ++i) {
                bool on_top = false;
                ap_creature_process_barrier_ptr(
                        creature_using, data + i, &on_top);
                if (on_top) {
                        is_standing = true;
                }
        }
        if (!is_standing) {
                creature_using->floating = true;
        }

        return 0;
}

int ap_box_box_collision_move(
        struct AP_PBox *barrial_box,
        struct AP_PBox *movable_box,
        bool *on_top)
{
        if (!barrial_box || !movable_box) {
                return AP_ERROR_INVALID_PARAMETER;
        }

        float min_dis[3] = { 0.0f };
        float dis[3] = { 0.0f };
        for (int i = 0; i < 3; ++i) {
                min_dis[i] = (barrial_box->size[i] + movable_box->size[i])/2.f;
                dis[i] = ap_absf(barrial_box->pos[i] - movable_box->pos[i]);
        }

        // x: 4, y: 2, z: 1
        char flag = 0x00;
        for (int i = 0; i < 3; ++i) {
                flag = flag << 1;
                if (dis[i] < min_dis[i]) {
                        flag++;
                }
        }
        if (on_top != NULL) {
                *on_top = (flag == 0x05 && (dis[1] - min_dis[1] < 0.01f));
        }
        // check if creature box is in barrial box or not
        if (flag != 0x07) {
                // LOGD("on_top = %d, flag = %d", *on_top, flag);
                return 0;
        }

        // get which face of the barriar is closest to the creature
        float max = 0.0f;
        int max_i = 0;
        for (int i = 0; i < 3; ++i) {
                dis[i] = ap_absf(dis[i] / barrial_box->size[i]);
                if (max < dis[i]) {
                        max = dis[i];
                        max_i = i;
                }
        }

        if (barrial_box->pos[max_i] < movable_box->pos[max_i]) {
                movable_box->pos[max_i] = barrial_box->pos[max_i]
                        + (barrial_box->size[max_i] + movable_box->size[max_i])
                        / 2.0f;
        } else {
                movable_box->pos[max_i] = barrial_box->pos[max_i]
                        - (barrial_box->size[max_i] + movable_box->size[max_i])
                        / 2.0f;
        }
        if (creature_using->floating) {
                LOGD("stop floating");
                creature_using->floating = false;
                if (on_top) {
                        *on_top = true;
                }
        }

        return 0;
}

int ap_barrier_set_pos(unsigned int id, float pos[3])
{
        struct AP_PBarrier *barrier = NULL;
        ap_barrier_get_ptr(id, &barrier);
        if (!barrier) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        memcpy(barrier->box.pos, pos, VEC3_SIZE);
        return 0;
}

int ap_barrier_set_size(unsigned int id, float size[3])
{
        struct AP_PBarrier *barrier = NULL;
        ap_barrier_get_ptr(id, &barrier);
        if (!barrier) {
                return AP_ERROR_INVALID_PARAMETER;
        }
        memcpy(barrier->box.size, size, VEC3_SIZE);
        return 0;
}