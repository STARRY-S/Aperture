#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include <assimp/cfileio.h>

#include "renderer.h"
#include "main.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "model.h"
#include "mesh.h"
#include "custom_io.h"

#define MODEL_FILE_NAME "backpack/backpack.obj"

void mouse_callback(double xpos, double ypos);
int SCREEN_WIDTH = 0, SCREEN_HEIGHT = 0;
struct Camera *camera = NULL;

float lastX = 400, lastY = 300;
double virtual_xpos = 0.0, virtual_ypox = 0.0;
bool firstMouse = true;
GLuint ourShader = 0;
struct Model model;

int setup()
{
    // compile shader
    ourShader = load_program("glsl/model_loading.vs.glsl",
                     "glsl/model_loading.fs.glsl");

    // init model
    GE_errorno = init_model(&model, MODEL_FILE_NAME, false);
    GE_CHECK(GE_errorno);

    // camera
    camera = initCamera();

    // depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // cull face
    glEnable(GL_CULL_FACE);

    return EXIT_SUCCESS;
}

int render()
{
    static float current_frame = 0.0f;
    // render
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(ourShader);
    // view/projection transformations
    mat4 view;
    mat4 projection;
    glm_mat4_identity(view);
    glm_mat4_identity(projection);
    GetViewMatrix(&view);

    glm_perspective(
            glm_rad(camera->zoom),
            (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
            0.1f, 100.0f, projection
    );

    shaderSetMat4(ourShader, "view", view[0]);
    shaderSetMat4(ourShader, "projection", projection[0]);

    // render the loaded model
    mat4 mat_model;
    glm_mat4_identity(mat_model);
    vec3 model_position = {0.0f, 0.0f, 0.0f};
    // translate it down so it's at the center of the scene
    glm_translate(mat_model, model_position);
    vec3 model_scale = {1.0f, 1.0f, 1.0f};
    // it's a bit too big for our scene, so scale it down
    glm_scale(mat_model, model_scale);
    // rotate cube by time.
    vec4 axis = {0.0f, 1.0f, 0.0f};
    glm_rotate(mat_model,
        glm_rad((float)current_frame * 50 + 20), axis);
    shaderSetMat4(ourShader, "model", (float *) mat_model);
    current_frame += 0.01f;

    // optimize depth test
    int iMobileType = getMobileType(getMobileName());
    bool bEnableMobileType = false;
    switch (iMobileType) {
        case GE_MOBILE_GOOGLE:
        case GE_MOBILE_X86:
            bEnableMobileType = false;
            break;
        default:
            bEnableMobileType = true;
    }
    shaderSetInt(ourShader, "optDepth", bEnableMobileType);

    GE_errorno = draw_model(&model, ourShader);
    GE_CHECK(GE_errorno);

    glBindVertexArray(0);
    return EXIT_SUCCESS;
}

int finish()
{
    // glDeleteVertexArrays(1, &cubeVAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteProgram(cubeShader);
    return EXIT_SUCCESS;
}

int resizeBuffer(int width, int height)
{
    LOGD("Resize buffer to width: %d, height: %d", width, height);
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    glViewport(0, 0, width, height);
    return 0;
}

void mouse_callback(double xpos, double ypos)
{
    if(firstMouse)
    {
        lastX = (float) xpos;
        lastY = (float) ypos;
        firstMouse = false;
    }

    float xoffset = (float) xpos - lastX;
    float yoffset = lastY - ypos; // 这里是相反的，因为y坐标从底部往顶部依次增大
    lastX = (float) xpos;
    lastY = (float) ypos;

    ProcessMouseMovement(xoffset, yoffset, true);
}
