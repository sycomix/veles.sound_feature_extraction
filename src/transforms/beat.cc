/*! @file beat.cc
 *  @brief Beat detection.
 *  @author Shapichev Alexey <a.shapichev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/beat.h"


namespace SoundFeatureExtraction {
namespace Transforms {

using BeatRaw16 = Beat<Formats::RawFormat16>;
using BeatRawF = Beat<Formats::RawFormatF>;
using BeatWindowF = Beat<Formats::WindowFormatF>;

REGISTER_TRANSFORM(BeatRaw16);
REGISTER_TRANSFORM(BeatRawF);
REGISTER_TRANSFORM(BeatWindowF);

}  // namespace Transforms
}  // namespace SoundFeatureExtraction
