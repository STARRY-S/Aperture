#include <GLES3/gl3.h>
#include <assimp/cfileio.h>

#include "ap_render.h"
#include "ap_utils.h"
#include "ap_camera.h"
#include "ap_shader.h"
#include "ap_texture.h"
#include "ap_model.h"
#include "ap_mesh.h"
#include "ap_custom_io.h"

int ap_render_general_initialize()
{
        return EXIT_SUCCESS;
}

int ap_render_finish()
{
        ap_camera_free();
        ap_shader_free();
        ap_model_free();
        ap_texture_free();
        ap_memory_release();

        return EXIT_SUCCESS;
}

int ap_resize_screen_buffer(int width, int height)
{
        LOGD("Resize buffer to width: %d, height: %d", width, height);
        ap_set_buffer(width, height);
        glViewport(0, 0, width, height);
        return 0;
}
