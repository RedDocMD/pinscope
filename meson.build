project('pinscope', 'c', 'cpp',
  version: '0.0.1',
  default_options: [
    'cpp_std=c++17'
  ]
)

gtkmm_dep = dependency('gtkmm-4.0')

program_name = 'pinscope'

cpp_sources = [
  'main.cpp',
  'pinscope.cpp',
  'window.cpp',
  'plot_area.cpp',
]

executable(program_name,
  cpp_sources,
  dependencies: gtkmm_dep,
  win_subsystem: 'windows',
)
