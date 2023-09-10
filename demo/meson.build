configure_file(
    input: 'include' / 'demo_config.h.in',
    output: 'demo_config.h',
    configuration: {
        'demo_datadir': get_option('prefix') / get_option('datadir') /
            'aperture_demo',
    },
)

executable(
    'aperture_demo',
    sources: files(
        'src' / 'demo_database.c',
        'src' / 'demo_light.c',
        'src' / 'demo_main.c',
        'src' / 'demo_utils.c',
    ),
    dependencies: [
        dependency('glfw3'),
        cc.find_library('m'),
    ],
    link_with: [
        aperture,
    ],
    include_directories: include_directories(
        'include',
        '../include',
        '../',
    ),
    install: true
)

meson.add_install_script('scripts/install_demo_data.sh')
