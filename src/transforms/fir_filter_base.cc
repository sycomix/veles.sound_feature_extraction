/*! @file convolution_filter_base.cc
 *  @brief Base class for all FIR filters.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#include "src/transforms/fir_filter_base.h"
#include "src/primitives/convolute.h"
#include "src/primitives/arithmetic-inl.h"

namespace SpeechFeatureExtraction {
namespace Transforms {

FirFilterBase::FirFilterBase() noexcept
: length_(DEFAULT_FILTER_LENGTH),
  windowType_(WINDOW_TYPE_HAMMING) {
  RegisterSetter("length", [&](const std::string& value) {
    int pv = Parse<int>("length", value);
    if (pv < MIN_FILTER_LENGTH || pv > MAX_FILTER_LENGTH) {
      return false;
    }
    length_ = pv;
    return true;
  });
  RegisterSetter("window", [&](const std::string& value) {
    auto wti = WindowTypeMap.find(value);
    if (wti == WindowTypeMap.end()) {
      return false;
    }
    windowType_ = wti->second;
    return true;
  });
}

void FirFilterBase::Initialize() const noexcept {
  filter_.resize(length_);
  for (int i = 0; i < length_; i++) {
    filter_[i] = WindowElement(windowType_, length_, i);
  }
  CalculateFilter(&filter_[0]);
  dataBuffer_.resize(inputFormat_->Size());
}

void FirFilterBase::InitializeBuffers(
    const BuffersBase<Formats::Raw16>& in,
    BuffersBase<Formats::Raw16>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size(),
                      in[0]->AlignmentOffset());
}

void FirFilterBase::Do(const BuffersBase<Formats::Raw16>& in,
                               BuffersBase<Formats::Raw16> *out)
const noexcept {
  for (size_t i = 0; i < in.Size(); i++) {
    int16_to_float(in[i]->Data.get(), inputFormat_->Size(), &dataBuffer_[0]);
    convolute(&dataBuffer_[0], inputFormat_->Size(),
              &filter_[0], filter_.size(),
              &dataBuffer_[0]);
    float_to_int16(&dataBuffer_[0], inputFormat_->Size(),
                   (*out)[i]->Data.get());
  }
}

}  // namespace Formats
}  // namespace SpeechFeatureExtraction
