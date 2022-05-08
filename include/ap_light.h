/**
 * @author STARRY-S (hxstarrys@gmail.com)
 * @brief Lighting stimulation
 *
 * @copyright Apache 2.0 - Copyright (c) 2022
 */

#ifndef AP_LIGHT_H
#define AP_LIGHT_H

#include "cglm/cglm.h"

#ifndef AP_LIGHT_POINT_NUM
#define AP_LIGHT_POINT_NUM 128
#endif // AP_LIGHT_POINT_NUM

#ifndef AP_LIGHT_PARAM_NUM
#define AP_LIGHT_PARAM_NUM 8
#endif  // AP_LIGHT_PARAM_NUM

typedef enum {
        AP_LIGHT_UNKNOWN = 0,
        AP_LIGHT_POINT = 0x2001,
        AP_LIGHT_DIRECTIONAL,
        AP_LIGHT_SPOT
} AP_Light_types;

struct AP_Light {
        int type;
        unsigned int id;

        float position[3];
        float direction[3];

        float ambient[3];
        float diffuse[3];
        float specular[3];

        /**
         * reserve parameters for light:
         * for AP_LIGHT_POINT:
         * [0] constant
         * [1] linear
         * [2] quadratic
         *
         * for AP_LIGHT_SPOT:
         * [0] contant
         * [1] linear
         * [2] quadratic
         * [3] cut_off
         * [4] outer_cut_off
         */
        float param[AP_LIGHT_PARAM_NUM];
};

static inline const char* ap_light_type_2_str(int type)
{
        switch (type)
        {
        case AP_LIGHT_POINT:
                return "point_light";
        case AP_LIGHT_DIRECTIONAL:
                return "direct_light";
        case AP_LIGHT_SPOT:
                return "spot_light";
        }
        return NULL;
}

/**
 * @brief Setup and generate a point light struct object
 * and store its data into vector.
 *
 * @param light_id [out] ID of the point light
 * @param position
 * @param ambient
 * @param diffuse
 * @param specular
 * @param param see the definition of struct AP_Light
 * @return int
 */
int ap_light_generate_point(
        unsigned int *light_id,
        float position[3],
        float ambient[3],
        float diffuse[3],
        float specular[3],
        float param[AP_LIGHT_PARAM_NUM]
);

int ap_light_setup_spot(
        float ambient[3],
        float diffuse[3],
        float specular[3],
        float param[AP_LIGHT_PARAM_NUM]
);

int ap_light_setup_directional(
        float direction[3],
        float ambient[3],
        float diffuse[3],
        float specular[3]
);

/**
 * @brief Send all of the data to the GPU after we generated
 * all of the light struct objects
 *
 * @return int
 */
int ap_light_send_data();

int ap_light_set_material_shininess(float shininess);

int ap_light_free();

#endif