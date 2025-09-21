
# msys
.PHONY: screenshots
screenshots:
	bash create_screenshots.sh

.PHONY: all
all: screenshots
	tks app:md -il -css index.css index.md

.PHONY: index
index:
	tks app:md -il -css index.css index.md

.PHONY: keyboard
keyboard:
	tks app:md -il -css index.css keyboard.md

.PHONY: faderport
faderport:
	tks app:md -il -css index.css faderport.md

.PHONY: montage
montage:
	tks app:md -il -css index.css montage.md

.PHONY: md
md: index keyboard faderport montage

# cygwin
.PHONY: thumbs
thumbs:
	(cd screenshots/thumb; sh create_thumbs.sh)
