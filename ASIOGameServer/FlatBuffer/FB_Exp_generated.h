// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_FBEXP_FB_H_
#define FLATBUFFERS_GENERATED_FBEXP_FB_H_

#include "flatbuffers/flatbuffers.h"

#include "FB_generated.h"

namespace FB {

struct Exp;

struct Exp FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_CURRENTEXP = 4,
    VT_REQEXP = 6
  };
  int32_t currentExp() const {
    return GetField<int32_t>(VT_CURRENTEXP, 0);
  }
  int32_t reqExp() const {
    return GetField<int32_t>(VT_REQEXP, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int32_t>(verifier, VT_CURRENTEXP) &&
           VerifyField<int32_t>(verifier, VT_REQEXP) &&
           verifier.EndTable();
  }
};

struct ExpBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_currentExp(int32_t currentExp) {
    fbb_.AddElement<int32_t>(Exp::VT_CURRENTEXP, currentExp, 0);
  }
  void add_reqExp(int32_t reqExp) {
    fbb_.AddElement<int32_t>(Exp::VT_REQEXP, reqExp, 0);
  }
  explicit ExpBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ExpBuilder &operator=(const ExpBuilder &);
  flatbuffers::Offset<Exp> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Exp>(end);
    return o;
  }
};

inline flatbuffers::Offset<Exp> CreateExp(
    flatbuffers::FlatBufferBuilder &_fbb,
    int32_t currentExp = 0,
    int32_t reqExp = 0) {
  ExpBuilder builder_(_fbb);
  builder_.add_reqExp(reqExp);
  builder_.add_currentExp(currentExp);
  return builder_.Finish();
}

inline const FB::Exp *GetExp(const void *buf) {
  return flatbuffers::GetRoot<FB::Exp>(buf);
}

inline const FB::Exp *GetSizePrefixedExp(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<FB::Exp>(buf);
}

inline bool VerifyExpBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<FB::Exp>(nullptr);
}

inline bool VerifySizePrefixedExpBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<FB::Exp>(nullptr);
}

inline void FinishExpBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FB::Exp> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedExpBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FB::Exp> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace FB

#endif  // FLATBUFFERS_GENERATED_FBEXP_FB_H_
