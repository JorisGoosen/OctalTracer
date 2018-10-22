TEMPLATE = subdirs

SUBDIRS = 	Octal \
  		Renderer \
		TextureRenderer

Renderer.depends = Octal

