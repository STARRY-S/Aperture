executable(
    'demo',
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
