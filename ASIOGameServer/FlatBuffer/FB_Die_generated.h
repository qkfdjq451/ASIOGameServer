// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_FBDIE_FB_H_
#define FLATBUFFERS_GENERATED_FBDIE_FB_H_

#include "flatbuffers/flatbuffers.h"

#include "FB_generated.h"

namespace FB {

struct Die;

struct Die FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_CODE = 4,
    VT_PLAYERTYPE = 6
  };
  int16_t code() const {
    return GetField<int16_t>(VT_CODE, 0);
  }
  PlayerType playerType() const {
    return static_cast<PlayerType>(GetField<int8_t>(VT_PLAYERTYPE, 0));
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int16_t>(verifier, VT_CODE) &&
           VerifyField<int8_t>(verifier, VT_PLAYERTYPE) &&
           verifier.EndTable();
  }
};

struct DieBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_code(int16_t code) {
    fbb_.AddElement<int16_t>(Die::VT_CODE, code, 0);
  }
  void add_playerType(PlayerType playerType) {
    fbb_.AddElement<int8_t>(Die::VT_PLAYERTYPE, static_cast<int8_t>(playerType), 0);
  }
  explicit DieBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  DieBuilder &operator=(const DieBuilder &);
  flatbuffers::Offset<Die> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Die>(end);
    return o;
  }
};

inline flatbuffers::Offset<Die> CreateDie(
    flatbuffers::FlatBufferBuilder &_fbb,
    int16_t code = 0,
    PlayerType playerType = PlayerType_Player) {
  DieBuilder builder_(_fbb);
  builder_.add_code(code);
  builder_.add_playerType(playerType);
  return builder_.Finish();
}

inline const FB::Die *GetDie(const void *buf) {
  return flatbuffers::GetRoot<FB::Die>(buf);
}

inline const FB::Die *GetSizePrefixedDie(const void *buf) {
  return flatbuffers::GetSizePrefixedRoot<FB::Die>(buf);
}

inline bool VerifyDieBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<FB::Die>(nullptr);
}

inline bool VerifySizePrefixedDieBuffer(
    flatbuffers::Verifier &verifier) {
  return verifier.VerifySizePrefixedBuffer<FB::Die>(nullptr);
}

inline void FinishDieBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FB::Die> root) {
  fbb.Finish(root);
}

inline void FinishSizePrefixedDieBuffer(
    flatbuffers::FlatBufferBuilder &fbb,
    flatbuffers::Offset<FB::Die> root) {
  fbb.FinishSizePrefixed(root);
}

}  // namespace FB

#endif  // FLATBUFFERS_GENERATED_FBDIE_FB_H_
