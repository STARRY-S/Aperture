/**
 * @file aperture.h
 * @author Starry Wang (hxstarrys@gmail.com)
 * @brief includes all aperture headers
 */
#ifndef APERTURE_H
#define APERTURE_H

// ap_* headers
#include "ap_audio.h"
#include "ap_camera.h"
#include "ap_custom_io.h"
#include "ap_cvector.h"
#include "ap_decode.h"
#include "ap_light.h"
#include "ap_math.h"
#include "ap_memory.h"
#include "ap_model.h"
// NOTE: network function is WIP
// #include "ap_network.h"
#include "ap_physic.h"
#include "ap_render.h"
#include "ap_shader.h"
#include "ap_sqlite.h"
#include "ap_texture.h"
#include "ap_utils.h"
#include "ap_vertex.h"

// OpenGL Headers
#if AP_PLATFORM_ANDROID
#include <GLES3/gl3.h>
#else
#include "glad/glad.h"
#endif // AP_PLATFORM_ANDROID

#endif