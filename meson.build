project('compile', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'werror=true'])

exe = executable('compile', [
  'src/main.c',

  'src/common/common.c',
  'src/common/arena.c',
  'src/common/error.c',
  'src/common/hash.c',

  'src/frontend/lexer.c',
  'src/frontend/parser.c',
  'src/frontend/ast_debug.c'

  #'src/codegen/codegen.c'
])