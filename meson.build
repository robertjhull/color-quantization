project('color-quantization-tool', 'cpp', default_options: ['cpp_std=c++17'])


compiler_args = ['-pedantic', '-Wno-ignored-attributes', '-Wno-long-long']

add_global_arguments(compiler_args, language : 'cpp')

src_dir = 'src'
build_dir = 'build'

pch = 'src/pch/cqt_pc.h'

source_files = [
    'src/main.cpp',
    'src/palette.cpp',
    'src/image.cpp',
    'src/dither.cpp',
    'src/quantization.cpp',
    'src/lodepng.cpp'
]

test_source_files = [
    'src/palette.cpp',
    'src/quantization.cpp',
    'test/test.cpp',
    'test/quantization.test.cpp',
]

external_include_dirs = include_directories(
    'external/Catch2', 
    'external/lodepng',
    )

eigen_dep = dependency('eigen3')
catch_dep = dependency('catch2')

executable('cqt',
    source_files,
    cpp_pch : pch,
    include_directories : [include_directories('src'), external_include_dirs], 
    dependencies: [eigen_dep])

test_exe = executable('unit_test',
    test_source_files,
    include_directories : [include_directories('src', 'test'), external_include_dirs],
    dependencies: [eigen_dep, catch_dep])

test('run_tests', test_exe)