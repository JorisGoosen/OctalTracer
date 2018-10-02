TEMPLATE = subdirs

SUBDIRS = Octal \
  Renderer

Renderer.depends = Octal

