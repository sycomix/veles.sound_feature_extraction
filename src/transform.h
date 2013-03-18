/*! @file transform.h
 *  @brief Transform parent class declaration.
 *  @author Markovtsev Vadim <v.markovtsev@samsung.com>
 *  @version 1.0
 *
 *  @section Notes
 *  This code partially conforms to <a href="http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml">Google C++ Style Guide</a>.
 *
 *  @section Copyright
 *  Copyright 2013 Samsung R&D Institute Russia
 */

#ifndef SRC_TRANSFORM_H_
#define SRC_TRANSFORM_H_

#include "src/config.h"
#include <string>
#include <unordered_map>  // NOLINT(build/include_order)
#include "src/buffer_format.h"
#include "src/buffers.h"
#include "src/parameters.h"

namespace SpeechFeatureExtraction {

class InvalidParameterNameException : public ExceptionBase {
 public:
  InvalidParameterNameException(const std::string& name,
                                const std::string& transformName)
  : ExceptionBase("There is no \"" + name + "\" parameter in " +
                  transformName + " supported parameters list.") {}
};

class InvalidParameterValueException : public ExceptionBase {
 public:
  InvalidParameterValueException(const std::string& name,
                                 const std::string& value,
                                 const std::string& transformName)
  : ExceptionBase("Parameter \"" + name + "\" is set to an invalid value \"" +
                  value + "\" for transform " + transformName + ".") {}
};

class Transform {
 public:
  virtual ~Transform() {}

  /// @defgroup PureVirt Pure virtual interface methods
  /// @{
  virtual const std::string& Name() const noexcept = 0;

  virtual const std::string& Description() const noexcept = 0;

  virtual const std::shared_ptr<BufferFormat> InputFormat() const noexcept = 0;

  virtual void SetInputFormat(const std::shared_ptr<BufferFormat>& format) = 0;

  virtual const std::shared_ptr<BufferFormat> OutputFormat()
      const noexcept = 0;

  virtual const std::unordered_map<std::string, ParameterTraits>&
  SupportedParameters() const noexcept = 0;

  virtual const std::unordered_map<std::string, std::string>&
  CurrentParameters() const noexcept = 0;

  virtual void SetParameters(
      const std::unordered_map<std::string, std::string>& parameters) = 0;

  virtual void Initialize() const noexcept = 0;

  virtual std::shared_ptr<Buffers> CreateOutputBuffers(const Buffers& in)
      const noexcept = 0;

  virtual void Do(const Buffers& in, Buffers* out) const noexcept = 0;
  /// @}

  /// @defgroup DefVirt Virtual methods which have defaults
  /// @{
  virtual bool HasInverse() const noexcept {
    return false;
  }

  virtual void DoInverse(const Buffers& in UNUSED,
                         Buffers* out UNUSED) const {
    std::unexpected();
  }
  /// @}

  /// @defgroup NonVirt Non-virtual methods
  /// @{
  std::shared_ptr<Transform> Clone() const noexcept;

  bool operator==(const Transform& other) const noexcept;

  bool IsInverse() const noexcept;

  std::string SafeName() const noexcept;

  std::string HtmlEscapedName() const noexcept;
  /// @}
};

}  // namespace SpeechFeatureExtraction
#endif  // SRC_TRANSFORM_H_
