mods = ['ibus', 'xim', 'scim']

if get_option('ecore_wl2')
  mods += ['wayland']
endif

foreach mod_name : mods
  mod_src = []
  mod_deps = []

  if mod_name == 'wayland' or get_option(mod_name) == true
    subdir(mod_name)
    config_h.set('BUILD_ECORE_IMF_'+mod_name.to_upper(), '1')

    mod_install_dir =  join_paths(dir_package_modules, mod_name, module_version)
    name = '_'.join(['imf_module', mod_name])

    mod = shared_module(name,
        mod_src,
        dependencies: mod_deps,
        install: true,
        install_dir : mod_install_dir
    )
    module_files += mod.full_path()
  endif
endforeach
