/**
 * @file ap_light.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief Multi-light definitions
 *
 */

#ifndef AP_LIGHT_H
#define AP_LIGHT_H

#ifndef AP_LIGHT_POINT_NUM
#define AP_LIGHT_POINT_NUM 128
#endif // AP_LIGHT_POINT_NUM

#ifndef AP_LIGHT_PARAM_NUM
#define AP_LIGHT_PARAM_NUM 8
#endif  // AP_LIGHT_PARAM_NUM

/**
 * @brief Light types definition
 */
typedef enum {
        AP_LIGHT_UNKNOWN = 0,
        /** point light */
        AP_LIGHT_POINT = 0x2001,
        /** directional light */
        AP_LIGHT_DIRECTIONAL,
        /** spot light */
        AP_LIGHT_SPOT
} AP_Light_types;

/**
 * @brief Light struct object
 */
struct AP_Light {
        /** @see AP_Light_types */
        int type;
        /** light id */
        unsigned int id;

        /** light position (for point light) */
        float position[3];
        /** light direction (for directional light) */
        float direction[3];

        /** ambient color */
        float ambient[3];
        /** diffuse color */
        float diffuse[3];
        /** specular color */
        float specular[3];

        /**
         * @brief parameters for light:
         *
         * for AP_LIGHT_POINT:
         * [0] constant,
         * [1] linear,
         * [2] quadratic
         *
         * for AP_LIGHT_SPOT:
         * [0] contant,
         * [1] linear,
         * [2] quadratic,
         * [3] cut_off,
         * [4] outer_cut_off
         */
        float param[AP_LIGHT_PARAM_NUM];
};

/**
 * @brief Convert light type to string
 *
 * @param type
 * @return const char*
 */
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

/**
 * @brief Setup spot light struct object
 *
 * @param ambient
 * @param diffuse
 * @param specular
 * @param param
 * @return int
 */
int ap_light_setup_spot(
        float ambient[3],
        float diffuse[3],
        float specular[3],
        float param[AP_LIGHT_PARAM_NUM]
);

/**
 * @brief Setup directional light struct object
 *
 * @param direction
 * @param ambient
 * @param diffuse
 * @param specular
 * @return int
 */
int ap_light_setup_directional(
        float direction[3],
        float ambient[3],
        float diffuse[3],
        float specular[3]
);

/**
 * @brief Render all lights
 *
 * @return int
 */
int ap_light_render();

/**
 * @brief Set shininess of material
 *
 * @param shininess
 * @return int
 */
int ap_light_set_material_shininess(float shininess);

/**
 * @brief release point light data by its ID
 *
 * @param id
 * @return int
 */
int ap_light_free_point_light(int id);

/**
 * @brief release all data for light rendering
 *
 * @return int
 */
int ap_light_free_all();

struct AP_Light* ap_light_get_point_light_ptr(int id);
struct AP_Light* ap_light_get_direct_light_ptr();
struct AP_Light* ap_light_get_spot_light_ptr();

#endif