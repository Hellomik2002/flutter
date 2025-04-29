

#ifndef FLUTTER_IMPELLER_COMPILER_MOBILE_SWITCHES_H_
#define FLUTTER_IMPELLER_COMPILER_MOBILE_SWITCHES_H_

#include <cstdint>
#include <iostream>
#include <map>
#include <memory>

#include "flutter/fml/file.h"
#include "fml/command_line.h"
#include "impeller/compiler/mobile/source_options_mobile.h"
#include "impeller/compiler/switches.h"
#include "impeller/compiler/types.h"
#include "impeller/compiler/utilities.h"

namespace impeller {
namespace compiler {

struct MobileSwitches : public Switches {
  std::map<std::string, std::string> fileMap;
  std::string shader_bundle;

  MobileSwitches(std::string shader_bundle_path,
                 std::map<std::string, std::string> file_map);

  bool AreValid(std::ostream& explain) const;

  /// A vector containing at least one valid platform.
  std::vector<TargetPlatform> PlatformsToCompile() const;

  // Creates source options from these switches for the specified
  // TargetPlatform. Uses SelectDefaultTargetPlatform if not specified.
  MobileSourceOptions CreateSourceOptions(
      std::optional<TargetPlatform> target_platform = std::nullopt) const;

  static void PrintHelp(std::ostream& stream);

 private:
  // Use |SelectDefaultTargetPlatform|.
  TargetPlatform target_platform_ = TargetPlatform::kUnknown;
  // Use |PlatformsToCompile|.
  std::vector<TargetPlatform> runtime_stages_;
};

}  // namespace compiler
}  // namespace impeller

#endif  // FLUTTER_IMPELLER_COMPILER_SWITCHES_H_
