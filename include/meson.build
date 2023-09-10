aperture_inc = include_directories('.')

configure_file(
    input: 'ap_config.h.in',
    output: 'ap_config.h',
    configuration: {
        'project_version': meson.project_version(),
        'project_name': meson.project_name(),
        'package_datadir': get_option('prefix') / get_option('datadir'),
    },
    install: true,
    install_dir: get_option('includedir') / 'aperture',
)

install_headers(
    files(
        'ap_audio.h',
        'ap_camera.h',
        'ap_custom_io.h',
        'ap_cvector.h',
        'ap_decode.h',
        'ap_light.h',
        'ap_math.h',
        'ap_memory.h',
        'ap_mesh.h',
        'ap_model.h',
        'ap_network.h',
        'ap_physic.h',
        'ap_render.h',
        'ap_shader.h',
        'ap_sqlite.h',
        'ap_texture.h',
        'ap_thread.h',
        'ap_utils.h',
        'ap_vertex.h',
    ),
    subdir: 'aperture',
)
install_headers(
    'glad' / 'glad.h',
    subdir: 'aperture' / 'glad',
)
install_headers(
    'stb' / 'stb_image.h',
    subdir: 'aperture' / 'stb',
)
