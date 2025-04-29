
#include <iostream>

#include "impeller/compiler/mobile/shader_bundle_mobile.h"
#include "impeller/compiler/mobile/switches_mobile.h"

#include "impeller/compiler/compiler.h"
#include "impeller/compiler/reflector.h"
#include "impeller/compiler/shader_bundle.h"
#include "impeller/compiler/utilities.h"
#include "impeller/runtime_stage/runtime_stage.h"
#include "impeller/shader_bundle/shader_bundle_flatbuffers.h"
#include "third_party/json/include/nlohmann/json.hpp"


#include <new>
#include <cstdlib>

void* operator new(std::size_t size) {
  void* ptr = std::malloc(size);
  if (!ptr) std::abort(); // потому что исключения отключены
  return ptr;
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
  return std::malloc(size);
}

void operator delete(void* ptr) noexcept {
  std::free(ptr);
}


namespace impeller {
namespace compiler {

static std::unique_ptr<fb::shaderbundle::BackendShaderT>
MobileGenerateShaderBackendFB(TargetPlatform target_platform,
                              MobileSourceOptions& options,
                              const std::string& shader_name,
                              const ShaderConfig& shader_config) {
  auto result = std::make_unique<fb::shaderbundle::BackendShaderT>();

  auto source_ptr = options.fileMap[shader_config.source_file_name];
  auto source_mapping = std::make_shared<fml::NonOwnedMapping>(
      reinterpret_cast<const uint8_t*>(source_ptr.data()), source_ptr.size());

  /// Override options.
  options.target_platform = target_platform;
  options.type = shader_config.type;
  options.source_language = shader_config.language;
  options.entry_point_name = EntryPointFunctionNameFromSourceName(
      shader_config.source_file_name, options.type, options.source_language,
      shader_config.entry_point);

  Reflector::Options reflector_options;
  reflector_options.target_platform = options.target_platform;
  reflector_options.entry_point_name = options.entry_point_name;
  reflector_options.shader_name = shader_name;

  Compiler compiler(source_mapping, options, reflector_options);
  if (!compiler.IsValid()) {
    std::cerr << "Compilation failed for bundled shader \"" << shader_name
              << "\"." << std::endl;
    std::cerr << compiler.GetErrorMessages() << std::endl;
    return nullptr;
  }

  auto reflector = compiler.GetReflector();
  if (reflector == nullptr) {
    std::cerr << "Could not create reflector for bundled shader \""
              << shader_name << "\"." << std::endl;
    return nullptr;
  }

  auto bundle_data = reflector->GetShaderBundleData();
  if (!bundle_data) {
    std::cerr << "Bundled shader information was nil for \"" << shader_name
              << "\"." << std::endl;
    return nullptr;
  }

  result = bundle_data->CreateFlatbuffer();
  if (!result) {
    std::cerr << "Failed to create flatbuffer for bundled shader \""
              << shader_name << "\"." << std::endl;
    return nullptr;
  }

  return result;
}

//

//
static std::unique_ptr<fb::shaderbundle::ShaderT> MobileGenerateShaderFB(
    MobileSourceOptions options,
    const std::string& shader_name,
    const ShaderConfig& shader_config) {
  auto result = std::make_unique<fb::shaderbundle::ShaderT>();
  result->name = shader_name;
  result->metal_ios = MobileGenerateShaderBackendFB(
      TargetPlatform::kMetalIOS, options, shader_name, shader_config);
  if (!result->metal_ios) {
    return nullptr;
  }
  result->metal_desktop = MobileGenerateShaderBackendFB(
      TargetPlatform::kMetalDesktop, options, shader_name, shader_config);
  if (!result->metal_desktop) {
    return nullptr;
  }
  result->opengl_es = MobileGenerateShaderBackendFB(
      TargetPlatform::kOpenGLES, options, shader_name, shader_config);
  if (!result->opengl_es) {
    return nullptr;
  }
  result->opengl_desktop = MobileGenerateShaderBackendFB(
      TargetPlatform::kOpenGLDesktop, options, shader_name, shader_config);
  if (!result->opengl_desktop) {
    return nullptr;
  }
  result->vulkan = MobileGenerateShaderBackendFB(
      TargetPlatform::kVulkan, options, shader_name, shader_config);
  if (!result->vulkan) {
    return nullptr;
  }
  return result;
}
//

//

std::optional<fb::shaderbundle::ShaderBundleT>
MobileGenerateShaderBundleFlatbuffer(const std::string& bundle_config_json,
                                     const MobileSourceOptions& options) {
  // --------------------------------------------------------------------------
  /// 1. Parse the bundle configuration.
  ///

  std::optional<ShaderBundleConfig> bundle_config =
      ParseShaderBundleConfig(bundle_config_json, std::cerr);
  if (!bundle_config) {
    return std::nullopt;
  }

  // --------------------------------------------------------------------------
  /// 2. Build the deserialized shader bundle.
  ///

  fb::shaderbundle::ShaderBundleT shader_bundle;

  for (const auto& [shader_name, shader_config] : bundle_config.value()) {
    std::unique_ptr<fb::shaderbundle::ShaderT> shader =
        MobileGenerateShaderFB(options, shader_name, shader_config);
    if (!shader) {
      return std::nullopt;
    }
    shader_bundle.shaders.push_back(std::move(shader));
  }

  return shader_bundle;
}

std::shared_ptr<fml::Mapping> MobileGenerateShaderBundle(
    MobileSwitches& switches) {
  auto shader_bundle = MobileGenerateShaderBundleFlatbuffer(
      switches.shader_bundle, switches.CreateSourceOptions());

  auto builder = std::make_shared<flatbuffers::FlatBufferBuilder>();
  builder->Finish(fb::shaderbundle::ShaderBundle::Pack(*builder.get(),
                                                       &shader_bundle.value()),
                  fb::shaderbundle::ShaderBundleIdentifier());
  auto mapping = std::make_shared<fml::NonOwnedMapping>(
      builder->GetBufferPointer(), builder->GetSize(),
      [builder](auto, auto) {});
  return mapping;
}

}  // namespace compiler
}  // namespace impeller