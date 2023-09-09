project(
    'Aperture',
    'c',
    default_options: [
        'c_std=c11',
    ],
    license: 'Apache-2.0',
    version: '0.2.2',
)

cc = meson.get_compiler('c')

configure_file(
    input: 'include' / 'ap_config.h.in',
    output: 'ap_config.h',
    configuration: {
        'project_version': meson.project_version(),
        'project_name': meson.project_name(),
        'datadir': get_option('prefix') / get_option('datadir'),
    }
)

# "install_dir" must be specified when "install" in a configure_file is true
configure_file(
    input: 'src' / 'ap_glsl' / 'ap_orthographic.fs.glsl',
    output: 'ap_orthographic.fs.glsl',
    copy: true,
    install: true,
    install_dir: get_option('datadir') / 'aperture' / 'ap_glsl',
)
configure_file(
    input: 'src' / 'ap_glsl' / 'ap_orthographic.vs.glsl',
    output: 'ap_orthographic.vs.glsl',
    copy: true,
    install: true,
    install_dir: get_option('datadir') / 'aperture' / 'ap_glsl',
)
configure_file(
    input: 'src' / 'ap_glsl' / 'ap_perspective.fs.glsl',
    output: 'ap_perspective.fs.glsl',
    copy: true,
    install: true,
    install_dir: get_option('datadir') / 'aperture' / 'ap_glsl',
)
configure_file(
    input: 'src' / 'ap_glsl' / 'ap_perspective.vs.glsl',
    output: 'ap_perspective.vs.glsl',
    copy: true,
    install: true,
    install_dir: get_option('datadir') / 'aperture' / 'ap_glsl',
)

aperture = library(
    'aperture',
    sources: files(
        'src' / 'ap_audio.c',
        'src' / 'ap_camera.c',
        'src' / 'ap_custom_io.c',
        'src' / 'ap_cvector.c',
        'src' / 'ap_cvector.c',
        'src' / 'ap_decode.c',
        'src' / 'ap_light.c',
        'src' / 'ap_memory.c',
        'src' / 'ap_mesh.c',
        'src' / 'ap_model.c',
        'src' / 'ap_network.c',
        'src' / 'ap_physic.c',
        'src' / 'ap_render.c',
        'src' / 'ap_shader.c',
        'src' / 'ap_sqlite.c',
        'src' / 'ap_texture.c',
        'src' / 'ap_thread.c',
        'src' / 'ap_utils.c',
        'src' / 'ap_vertex.c',
        'src' / 'glad.c',
    ),
    dependencies: [
        dependency('glfw3'),
        dependency('assimp'),
        dependency('freetype2'),
        dependency('openal'),
        dependency('libavutil'),
        dependency('libavcodec'),
        dependency('libavdevice'),
        dependency('libavfilter'),
        dependency('libavformat'),
        dependency('libswresample'),
        dependency('libswscale'),
        dependency('sqlite3'),
        dependency('libcurl'),
        cc.find_library('alut'),
        cc.find_library('m'),
    ],
    include_directories: include_directories('include'),
    install: true,
)

subdir('demo')
