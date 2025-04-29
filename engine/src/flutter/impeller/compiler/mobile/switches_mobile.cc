#include "impeller/compiler/mobile/switches_mobile.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <map>

#include "flutter/fml/file.h"
#include "fml/command_line.h"
#include "impeller/compiler/types.h"
#include "impeller/compiler/utilities.h"

namespace impeller {
namespace compiler {

static const std::map<std::string, TargetPlatform> kKnownPlatforms = {
    {"metal-desktop", TargetPlatform::kMetalDesktop},
    {"metal-ios", TargetPlatform::kMetalIOS},
    {"vulkan", TargetPlatform::kVulkan},
    {"opengl-es", TargetPlatform::kOpenGLES},
    {"opengl-desktop", TargetPlatform::kOpenGLDesktop},
};

static const std::map<std::string, TargetPlatform> kKnownRuntimeStages = {
    {"sksl", TargetPlatform::kSkSL},
    {"runtime-stage-metal", TargetPlatform::kRuntimeStageMetal},
    {"runtime-stage-gles", TargetPlatform::kRuntimeStageGLES},
    {"runtime-stage-gles3", TargetPlatform::kRuntimeStageGLES3},
    {"runtime-stage-vulkan", TargetPlatform::kRuntimeStageVulkan},
};

static const std::map<std::string, SourceType> kKnownSourceTypes = {
    {"vert", SourceType::kVertexShader},
    {"frag", SourceType::kFragmentShader},
    {"comp", SourceType::kComputeShader},
};
MobileSwitches::MobileSwitches(std::string shader_bundle_path,
                               std::map<std::string, std::string> file_map)
    : fileMap(std::move(file_map)),
      shader_bundle(std::move(shader_bundle_path)) {}

MobileSourceOptions MobileSwitches::CreateSourceOptions(
    std::optional<TargetPlatform> target_platform) const {
  MobileSourceOptions options;
  options.fileMap = fileMap;
  options.target_platform =
      target_platform.value_or(SelectDefaultTargetPlatform());
  options.source_language = source_language;
  if (input_type == SourceType::kUnknown) {
    options.type = SourceTypeFromFileName(source_file_name);
  } else {
    options.type = input_type;
  }
  //   options.working_directory = working_directory;
  options.file_name = source_file_name;
  options.include_dirs = include_directories;
  options.defines = defines;
  options.entry_point_name = EntryPointFunctionNameFromSourceName(
      source_file_name, options.type, options.source_language, entry_point);
  options.json_format = json_format;
  options.gles_language_version = gles_language_version;
  options.metal_version = metal_version;
  options.use_half_textures = use_half_textures;
  options.require_framebuffer_fetch = require_framebuffer_fetch;
  return options;
}
}  // namespace compiler
}  // namespace impeller
