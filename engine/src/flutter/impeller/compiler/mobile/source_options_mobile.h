#ifndef FLUTTER_IMPELLER_COMPILER_MOBILE_SOURCE_OPTIONS_H_
#define FLUTTER_IMPELLER_COMPILER_MOBILE_SOURCE_OPTIONS_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "flutter/fml/mapping.h"
#include "flutter/fml/unique_fd.h"
#include "impeller/compiler/include_dir.h"
#include "impeller/compiler/source_options.h"
#include "impeller/compiler/types.h"

namespace impeller {
namespace compiler {

struct MobileSourceOptions : public SourceOptions {
 public:
  std::map<std::string, std::string> fileMap;
};

}  // namespace compiler
}  // namespace impeller

#endif  // FLUTTER_IMPELLER_COMPILER_SOURCE_OPTIONS_H_
