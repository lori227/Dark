// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: CodeMessage.proto

#ifndef PROTOBUF_INCLUDED_CodeMessage_2eproto
#define PROTOBUF_INCLUDED_CodeMessage_2eproto

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4946)
#endif

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_CodeMessage_2eproto LIBPROTOC_EXPORT

namespace protobuf_CodeMessage_2eproto {
// Internal implementation detail -- do not use these members.
struct LIBPROTOC_EXPORT TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void LIBPROTOC_EXPORT AddDescriptors();
}  // namespace protobuf_CodeMessage_2eproto
namespace KFMsg {
}  // namespace KFMsg
namespace KFMsg {

enum ProjectCodeEnum {
  LogicError = 0,
  MatchRequestOk = 31001,
  MatchAlreadyWait = 31002,
  MatchInRoom = 31003,
  MatchNotInMatch = 31004,
  MatchCancelOk = 31005,
  MatchAffirmTimeout = 31007,
  RoomAllotBattle = 31008,
  MatchIdError = 31009,
  HeroDivisorError = 31101,
  HeroDivisorExist = 31102,
  HeroRecruitRefresh = 31103,
  HeroRecruitRefreshFreeError = 31104,
  HeroRecruitRefreshTypeError = 31105,
  HeroRecruitNotExist = 31106,
  HeroRecruitCostError = 31107,
  HeroRecruitDataError = 31108,
  HeroRecruitOk = 31109,
  HeroDivisorMaxCount = 31110,
  HeroNotExist = 31111,
  HeroTeamIndexError = 31112,
  HeroTeamDataInvalid = 31113,
  HeroNotInHeroList = 31114,
  HeroLevelIsMax = 31115,
  HeroLockNotChange = 31116,
  HeroIsLocked = 31117,
  HeroLockFlagError = 31118,
  HeroNameNotChange = 31119,
  HeroSkillNotChange = 31120,
  HeroSkillIndexError = 31121,
  HeroHpIsMax = 31122,
  HeroTransferDataError = 31123,
  HeroLevelNotEnough = 31124,
  HeroTransferCostError = 31125,
  HeroCanNotTransfer = 31126,
  HeroProfessionIsError = 31127,
  HeroNameEmpty = 31128,
  HeroIsDead = 31131,
  HeroRaceIsError = 31132,
  HeroDivisorNotUnlock = 31133,
  HeroRecruitRefreshCount = 31134,
  HeroRecruitRefreshGenerate = 31135,
  HeroRecruitRefreshWeight = 31136,
  HeroRefreshCountNotEnough = 31137,
  HeroRecruitRefreshSuc = 31138,
  HeroDurabilityNotZero = 31140,
  ContractTimeFormulaError = 31141,
  ContractPriceFormulaError = 31142,
  ContractTimeIsZero = 31143,
  HeroTeamDurabilityNotEnough = 31144,
  HeroContractSuc = 31145,
  HeroRetireSuc = 31146,
  SpecialHeroCanNotRemove = 31147,
  TrainCampLevelError = 31200,
  TrainCampIndexError = 31201,
  TrainCampHeroNotExist = 31202,
  TrainCampHeroIsExist = 31203,
  BuildFuncNotActive = 31204,
  MainCampSkinNotExist = 31205,
  MainCampSkinNotActive = 31206,
  ClinicSettingError = 31207,
  ClinicItemIsNotEnough = 31208,
  ClinicMoneyIsError = 31209,
  TrainCampSettingError = 31210,
  TrainCampIsFinished = 31211,
  GranarySettingError = 31212,
  GranaryHaveNotItem = 31213,
  SmithyConsumeNotEnough = 31214,
  SmithySettingError = 31215,
  SmithyHaveNotItem = 31216,
  SmithyStoreIsMax = 31217,
  SmithyWeaponSettingError = 31218,
  SmithyMakeNumError = 31219,
  SmithyMakeNumNotEnough = 31220,
  ClinicCountIsNotEnough = 31221,
  ClinicNotNeedCure = 31222,
  BuildLevelIsMax = 31224,
  BuildInUpgradeTime = 31225,
  BuildNoFreeUpgradeList = 31226,
  BuildUpgradeLackCondition = 31227,
  BuildNotInUpgradeTime = 31228,
  BuildLevelSettingError = 31229,
  TechnologySettingError = 31230,
  TechnologyLackPreCondition = 31231,
  TechnologyIsUnlock = 31232,
  TechnologyIslock = 31233,
  BuildUpgradeSuc = 31234,
  TrainNotFinish = 31235,
  SmithyMakeLevelLimit = 31236,
  FormulaParamError = 31237,
  GranaryBuyCountLimit = 31238,
  GranaryBuySettingError = 31239,
  GranaryBuyParamError = 31240,
  BuildCanNotUpgrade = 31241,
  RealmMapError = 31301,
  RealmLevelError = 31302,
  RealmNotInExplore = 31305,
  PVEIdError = 31306,
  PVEAlready = 31307,
  RealmAlready = 31308,
  RealmNotInStatus = 31309,
  PVENotInStatus = 31310,
  PVEHeroTeamEmpty = 31311,
  PVEHeroTeamExist = 31312,
  PVEMapIdError = 31313,
  RealmDataError = 31314,
  RealmIdMismatch = 31315,
  RealmNotTown = 31316,
  RealmInTown = 31317,
  RealmHeroTeamEmpty = 31318,
  PVECanNotFlee = 31319,
  NpcGroupError = 31320,
  GambleIdError = 31321,
  GambleParamError = 31323,
  ExchangeIdError = 31324,
  ExchangeNotRune = 31325,
  ExchangeLevelError = 31326,
  SelectIdError = 31327,
  SelectCountError = 31328,
  SelectUUIDError = 31329,
  SelectRecordError = 31330,
  MultiEventIdError = 31331,
  MultiEventIndexError = 31332,
  NpcRandError = 31333,
  ChapterIdError = 32401,
  ChapterNotOpen = 32402,
  ChapterEventError = 32403,
  ChapterDefaultError = 32404,
  ChapterStatusIdError = 32405,
  ChapterExecuteFailed = 32406,
  ItemWeaponOnly = 32500,
  ItemWeaponRaceLimit = 32501,
  ItemWeaponHeroLimit = 32502,
  ItemWeaponLevelLimit = 32503,
  ItemWeaponFailed = 32504,
  ItemWeaponOk = 32505,
  ItemWeaponNotExist = 32506,
  ItemUnWeaponFailed = 32507,
  ItemUnWeaponOk = 32508,
  ItemUpdateDurabilityError = 32509,
  ItemCanNotUseToHero = 32510,
  RuneSameTypeLimit = 32514,
  RunePutOnSuc = 32515,
  RuneTakeOffSuc = 32516,
  PlayerHeadIconSetOK = 32601,
  PlayerFactionSetOK = 32602,
  PlayerHeadIconNotExist = 32603,
  PlayerFactionNotExist = 32604,
  PlayerHeadIconNotHad = 32605,
  StorySettingNotExist = 32700,
  StoryNotExist = 32701,
  StorySequenceIsError = 32702,
  StoryTriggerTypeLimit = 32703,
  DialogueSettingNotExist = 32704,
  DialogueNotExist = 32705,
  DialogueHaveNotBranch = 32706,
  DialogueBranchNotExist = 32707,
  DialogueBranchLackCondition = 32708,
  BranchSettingNotExist = 32709,
  DialogueBranchSelectSuc = 32710,
  StoryHaveDialogueBranch = 32711,
  StorySequenceNotFinish = 32712,
  StoryIsNotMainStory = 32713,
  TaskChainSettingNotExist = 32800,
  ProjectCodeEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  ProjectCodeEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool ProjectCodeEnum_IsValid(int value);
const ProjectCodeEnum ProjectCodeEnum_MIN = LogicError;
const ProjectCodeEnum ProjectCodeEnum_MAX = TaskChainSettingNotExist;
const int ProjectCodeEnum_ARRAYSIZE = ProjectCodeEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* ProjectCodeEnum_descriptor();
inline const ::std::string& ProjectCodeEnum_Name(ProjectCodeEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    ProjectCodeEnum_descriptor(), value);
}
inline bool ProjectCodeEnum_Parse(
    const ::std::string& name, ProjectCodeEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ProjectCodeEnum>(
    ProjectCodeEnum_descriptor(), name, value);
}
// ===================================================================


// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace KFMsg

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::KFMsg::ProjectCodeEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::ProjectCodeEnum>() {
  return ::KFMsg::ProjectCodeEnum_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)


#ifdef _MSC_VER
	#pragma warning(  pop  )
#endif
#endif  // PROTOBUF_INCLUDED_CodeMessage_2eproto
