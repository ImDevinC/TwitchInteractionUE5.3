uepath = "$$HOME/.local/opt/unreal-engine/Engine/Build/BatchFiles/RunUAT.sh"
cwd = $(shell pwd)
outdir = "build"

.PHONY: build
build:
	$(uepath) BuildPlugin -Plugin="$(cwd)/TwitchInteractionUE.uplugin" -Package="$(cwd)/$(outdir)"
