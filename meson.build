project('color-quantization-tool', 'cpp', default_options: ['cpp_std=c++17'])


compiler_args = ['-pedantic', '-Wno-ignored-attributes', '-Wno-long-long']

add_global_arguments(compiler_args, language : 'cpp')

src_dir = 'src'
build_dir = 'build'

pch = 'src/pch/cqt_pch.h'

source_files = files([
    'src/main.cpp',
    'src/palette.cpp',
    'src/lodepng.cpp',
    'src/image.cpp',
    'src/dither.cpp',
    'src/quantization.cpp',
])

test_source_files = files([
    'src/palette.cpp',
    'src/quantization.cpp',
    'src/lodepng.cpp',
    'src/image.cpp',
    'test/test.cpp',
    'test/quantization.test.cpp',
    'test/palette.test.cpp',
    'test/image.test.cpp'
])

eigen_dep = dependency('eigen3')
catch_dep = dependency('catch2-with-main')

executable('cqt',
    sources : [source_files],
    cpp_pch : pch,
    include_directories : include_directories('src'), 
    dependencies: [eigen_dep])

test_exe = executable('unit_test',
    sources : [test_source_files],
    cpp_pch : pch,
    include_directories : include_directories('src', 'test'),
    dependencies: [eigen_dep, catch_dep])

test('run_tests', test_exe, args : ['--success', '--abortx 5'])