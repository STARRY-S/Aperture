#include <GLES3/gl3.h>
#include <assimp/cfileio.h>

#include "renderer.h"
#include "ge_utils.h"
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
GLuint lightShader = 0;
GLuint cubeShader = 0;
GLuint lightCubeVAO = 0;
struct Model model;

vec3 light_position = { 3.0f, 3.0f, 3.0f };

int setup()
{
    // compile shader
    lightShader = load_program(
        "glsl/model_light.vs.glsl",
        "glsl/model_light.fs.glsl"
    );

    cubeShader = load_program(
        "glsl/cube_light.vs.glsl",
        "glsl/cube_light.fs.glsl"
    );

    // camera
    camera = initCamera();

    glUseProgram(lightShader);
    shaderSetInt(lightShader, "material.diffuse", 0);
    shaderSetInt(lightShader, "material.specular", 1);

    // TODO: Seperate the light setup param in individual function
    vec3 light_ambient = { 1.0f, 1.0f, 1.0f };
    vec3 light_diffuse = { 1.0f, 1.0f, 1.0f };
    vec3 light_specular = { 4.0f, 4.0f, 4.0f };

    shaderSetVec3(lightShader, "viewPos", camera->position);
    shaderSetVec3(lightShader, "light.position", light_position);
    shaderSetVec3(lightShader, "light.ambient", light_ambient);
    shaderSetVec3(lightShader, "light.diffuse", light_diffuse);
    shaderSetVec3(lightShader, "light.specular", light_specular);
    shaderSetFloat(lightShader, "material.shininess", 32.0f);

    shaderSetFloat(lightShader, "light.constant", 1.0f);
    shaderSetFloat(lightShader, "light.linear", 0.09f);
    shaderSetFloat(lightShader, "light.quadratic", 0.032f);

    glUseProgram(cubeShader);
    // nothing...
    glUseProgram(0);

    // light cube initialize
    const float* cubeVertices = getCubeVertices();
    GLuint VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER, getCubeVertivesLength(), cubeVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // init model
    GE_CHECK( init_model(&model, MODEL_FILE_NAME, false) );

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
    glUseProgram(lightShader);

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

    shaderSetMat4(lightShader, "view", view[0]);
    shaderSetMat4(lightShader, "projection", projection[0]);

    // render the loaded model
    mat4 mat_model;
    glm_mat4_identity(mat_model);
    vec3 model_position = { 0.0f, 0.0f, 0.0f };
    // translate it down so it's at the center of the scene
    glm_translate(mat_model, model_position);
    vec3 model_scale = { 1.0f, 1.0f, 1.0f };
    // it's a bit too big for our scene, so scale it down
    glm_scale(mat_model, model_scale);
    // rotate model by time.
    vec4 axis = {0.0f, 1.0f, 0.0f};
    glm_rotate(mat_model, glm_rad((float) current_frame), axis);
    shaderSetMat4(lightShader, "model", (float *) mat_model);
    current_frame += 0.2f;

    // optimize depth test
    bool bEnableMobileType = false;

    #ifdef __ANDROID__
    int iMobileType = getMobileType(getMobileName());
    switch (iMobileType) {
        case GE_MOBILE_GOOGLE:
        case GE_MOBILE_X86:
            bEnableMobileType = false;
            break;
        default:
            bEnableMobileType = true;
    }
    #endif // __ANDROID__

    shaderSetInt(lightShader, "optDepth", bEnableMobileType);

    GE_CHECK( draw_model(&model, lightShader) );

    // render the lamp cube
    glUseProgram(cubeShader);
    shaderSetMat4(cubeShader, "projection", projection[0]);
    shaderSetMat4(cubeShader, "view", view[0]);
    glm_mat4_identity(mat_model);
    glm_translate(mat_model, light_position);
    vec3 light_scale = { 0.50f, 0.50f, 0.50f };
    glm_scale(mat_model, light_scale);  // smaller
    shaderSetMat4(lightShader, "model", mat_model[0]);
    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

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
    LOGD("Resize buffer to width: %d, height: %d\n", width, height);
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
