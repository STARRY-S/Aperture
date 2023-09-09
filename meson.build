project('Aperture', 'c')

cc = meson.get_compiler('c')

glfw_dep = dependency('glfw3')
assimp_dep = dependency('assimp')
freetype_dep = dependency('freetype2')
openal_dep = dependency('openal')
avutil_dep = dependency('libavutil')
avcodec_dep = dependency('libavcodec')
avdevice_dep = dependency('libavdevice')
avfilter_dep = dependency('libavfilter')
avformat_dep = dependency('libavformat')
swresample_dep = dependency('libswresample')
swscale_dep = dependency('libswscale')
sqlite_dep = dependency('sqlite3')
curl_dep = cc.find_library('curl')
alut_dep = cc.find_library('alut')
m_dep = cc.find_library('m')

deps = [
    glfw_dep,
    assimp_dep,
    freetype_dep,
    openal_dep,
    avutil_dep,
    avcodec_dep,
    avdevice_dep,
    avfilter_dep,
    avformat_dep,
    swresample_dep,
    swscale_dep,
    sqlite_dep,
    curl_dep,
    alut_dep,
    m_dep,
]

src = [
    'src/ap_audio.c',
    'src/ap_camera.c',
    'src/ap_custom_io.c',
    'src/ap_cvector.c',
    'src/ap_cvector.c',
    'src/ap_decode.c',
    'src/ap_light.c',
    'src/ap_memory.c',
    'src/ap_mesh.c',
    'src/ap_model.c',
    'src/ap_network.c',
    'src/ap_physic.c',
    'src/ap_render.c',
    'src/ap_shader.c',
    'src/ap_sqlite.c',
    'src/ap_texture.c',
    'src/ap_thread.c',
    'src/ap_utils.c',
    'src/ap_vertex.c',
    'src/glad.c',
]

include_dir = include_directories('include')

libaperture = library(
    'aperture',
    sources : src,
    include_directories : include_dir,
    dependencies: deps,
)

demo_src = [
    'demo/demo_database.c',
    'demo/demo_light.c',
    'demo/demo_main.c',
    'demo/demo_utils.c',
]

demo_include_dir = include_directories('demo')

executable(
    'demo',
    sources : demo_src,
    include_directories : [
        demo_include_dir,
        include_dir,
    ],
    link_with : libaperture,
    dependencies : [
        glfw_dep,
        m_dep,
    ],
)
