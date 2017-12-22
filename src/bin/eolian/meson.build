eolian_gen_src = [
  'main.c',
  'main.h',
  'types.c',
  'types.h',
  'headers.c',
  'headers.h',
  'sources.c',
  'sources.h',
  'docs.c',
  'docs.h'
]

eolian_gen_native = executable('eolian_gen',
        eolian_gen_src,
        dependencies: eolian,
        install: true
)

if meson.is_cross_build()
  eolian_gen = find_program('eolian_gen')
else
  eolian_gen = eolian_gen_native
endif
