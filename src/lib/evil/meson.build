if target_machine.system() == 'windows'
  evil_header_src = [
  'Evil.h',
  'evil_dlfcn.h',
  'evil_fcntl.h',
  'evil_langinfo.h',
  'evil_locale.h',
  'evil_macro.h',
  'evil_macro_pop.h',
  'evil_macro_wrapper.h',
  'evil_main.h',
  'evil_stdio.h',
  'evil_stdlib.h',
  'evil_string.h',
  'evil_time.h',
  'evil_unistd.h',
  'evil_util.h',
  'dirent.h',
  'fnmatch.h',
  'pwd.h',
  'regex/regex.h'
  ]
  evil_header_sys_src = [join_paths('sys','mman.h')]

  evil_src = [
  'evil_dirent.c',
  'evil_dlfcn.c',
  'evil_fcntl.c',
  'evil_fnmatch.c',
  'evil_fnmatch_list_of_states.c',
  'evil_langinfo.c',
  'evil_locale.c',
  'evil_link_xp.cpp',
  'evil_main.c',
  'evil_mman.c',
  'evil_pwd.c',
  'evil_stdio.c',
  'evil_stdlib.c',
  'evil_string.c',
  'evil_time.c',
  'evil_unistd.c',
  'evil_util.c',
  'evil_private.h',
  'evil_fnmatch_private.h',
  ]

  subdir('regex')

  config_h.set('_WIN32_WINNT', '0x0601')
  config_h.set('WINVER', '0x0601')

  evil_lib = library('evil', evil_src,
    include_directories : [config_dir, include_directories('regex')],
  )

  evil = declare_dependency(
    include_directories: include_directories('.'),
    link_with: evil_lib,
  )
else
  evil = declare_dependency()
endif
