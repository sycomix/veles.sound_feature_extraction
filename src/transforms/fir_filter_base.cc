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
#include <simd/arithmetic-inl.h>
#include <simd/convolute.h>
#include <mutex>

namespace SoundFeatureExtraction {
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
    auto wti = kWindowTypeMap.find(value);
    if (wti == kWindowTypeMap.end()) {
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

  convolutionHandles_.resize(MaxThreadsNumber());
  for (int i = 0; i < MaxThreadsNumber(); i++) {
    convolutionHandles_[i].handle = std::shared_ptr<ConvoluteHandle>(
        new ConvoluteHandle(convolute_initialize(inputFormat_->Size(),
                                                 filter_.size())),
        [](ConvoluteHandle* ptr) {
          convolute_finalize(*ptr);
          delete ptr;
        }
    );
    convolutionHandles_[i].mutex = std::make_shared<std::mutex>();
  }
}

void FirFilterBase::OnFormatChanged() {
  outputFormat_->SetSize(inputFormat_->Size() + length_ - 1);
}

void FirFilterBase::InitializeBuffers(
    const BuffersBase<Formats::RawF>& in,
    BuffersBase<Formats::RawF>* buffers) const noexcept {
  buffers->Initialize(in.Size(), outputFormat_->Size(),
                      in[0].AlignmentOffset());
}

void FirFilterBase::Do(const Formats::RawF& in,
                       Formats::RawF *out)
const noexcept {
  for (auto hp : convolutionHandles_) {
    if (hp.mutex->try_lock()) {
      convolute(*hp.handle, in.Data.get(), &filter_[0], out->Data.get());
      hp.mutex->unlock();
      break;
    }
  }
}

}  // namespace Formats
}  // namespace SoundFeatureExtraction
