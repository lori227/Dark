// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: EnumMessage.proto

#ifndef PROTOBUF_INCLUDED_EnumMessage_2eproto
#define PROTOBUF_INCLUDED_EnumMessage_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_protobuf_EnumMessage_2eproto LIBPROTOC_EXPORT

namespace protobuf_EnumMessage_2eproto {
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
}  // namespace protobuf_EnumMessage_2eproto
namespace KFMsg {
}  // namespace KFMsg
namespace KFMsg {

enum ConstDefineEnum {
  UnknowConst = 0,
  InfiniteStep = 99999,
  InfiniteTurn = 99999,
  InfiniteBuff = 10000,
  AreaCamp = 1,
  AreaRealm = 2,
  ConstDefineEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  ConstDefineEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool ConstDefineEnum_IsValid(int value);
const ConstDefineEnum ConstDefineEnum_MIN = UnknowConst;
const ConstDefineEnum ConstDefineEnum_MAX = InfiniteStep;
const int ConstDefineEnum_ARRAYSIZE = ConstDefineEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* ConstDefineEnum_descriptor();
inline const ::std::string& ConstDefineEnum_Name(ConstDefineEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    ConstDefineEnum_descriptor(), value);
}
inline bool ConstDefineEnum_Parse(
    const ::std::string& name, ConstDefineEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ConstDefineEnum>(
    ConstDefineEnum_descriptor(), name, value);
}
enum CampEnum {
  UnknowCamp = 0,
  RedCamp = 1,
  BlueCamp = 2,
  CampEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  CampEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool CampEnum_IsValid(int value);
const CampEnum CampEnum_MIN = UnknowCamp;
const CampEnum CampEnum_MAX = BlueCamp;
const int CampEnum_ARRAYSIZE = CampEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* CampEnum_descriptor();
inline const ::std::string& CampEnum_Name(CampEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    CampEnum_descriptor(), value);
}
inline bool CampEnum_Parse(
    const ::std::string& name, CampEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<CampEnum>(
    CampEnum_descriptor(), name, value);
}
enum DivisorEnum {
  UnknowDivisor = 0,
  DivisorOfRace = 1,
  DivisorOfMove = 2,
  DivisorOfWeapon = 3,
  DivisorEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  DivisorEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool DivisorEnum_IsValid(int value);
const DivisorEnum DivisorEnum_MIN = UnknowDivisor;
const DivisorEnum DivisorEnum_MAX = DivisorOfWeapon;
const int DivisorEnum_ARRAYSIZE = DivisorEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* DivisorEnum_descriptor();
inline const ::std::string& DivisorEnum_Name(DivisorEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    DivisorEnum_descriptor(), value);
}
inline bool DivisorEnum_Parse(
    const ::std::string& name, DivisorEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<DivisorEnum>(
    DivisorEnum_descriptor(), name, value);
}
enum HeroLockEnum {
  NoLock = 0,
  HeroListLock = 1,
  CountLock = 2,
  HeroLockEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  HeroLockEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool HeroLockEnum_IsValid(int value);
const HeroLockEnum HeroLockEnum_MIN = NoLock;
const HeroLockEnum HeroLockEnum_MAX = CountLock;
const int HeroLockEnum_ARRAYSIZE = HeroLockEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* HeroLockEnum_descriptor();
inline const ::std::string& HeroLockEnum_Name(HeroLockEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    HeroLockEnum_descriptor(), value);
}
inline bool HeroLockEnum_Parse(
    const ::std::string& name, HeroLockEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<HeroLockEnum>(
    HeroLockEnum_descriptor(), name, value);
}
enum HeroRemoveEnum {
  RemoveUnknown = 0,
  Dismissal = 101,
  Retirement = 102,
  HeroRemoveEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  HeroRemoveEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool HeroRemoveEnum_IsValid(int value);
const HeroRemoveEnum HeroRemoveEnum_MIN = RemoveUnknown;
const HeroRemoveEnum HeroRemoveEnum_MAX = Retirement;
const int HeroRemoveEnum_ARRAYSIZE = HeroRemoveEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* HeroRemoveEnum_descriptor();
inline const ::std::string& HeroRemoveEnum_Name(HeroRemoveEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    HeroRemoveEnum_descriptor(), value);
}
inline bool HeroRemoveEnum_Parse(
    const ::std::string& name, HeroRemoveEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<HeroRemoveEnum>(
    HeroRemoveEnum_descriptor(), name, value);
}
enum BuildEnum {
  InvalidBuild = 0,
  HeroList = 200,
  HeroTeam = 201,
  MainBuild = 202,
  RecruitBuild = 203,
  ClinicBuild = 204,
  TrainBuild = 205,
  OperaBuild = 206,
  SmithyBuild = 207,
  ShopBuild = 208,
  InheritBuild = 209,
  CemeteryBuild = 210,
  OuterBuild = 211,
  WarehouseBuild = 212,
  GranaryBuild = 213,
  BuildEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  BuildEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool BuildEnum_IsValid(int value);
const BuildEnum BuildEnum_MIN = InvalidBuild;
const BuildEnum BuildEnum_MAX = GranaryBuild;
const int BuildEnum_ARRAYSIZE = BuildEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* BuildEnum_descriptor();
inline const ::std::string& BuildEnum_Name(BuildEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    BuildEnum_descriptor(), value);
}
inline bool BuildEnum_Parse(
    const ::std::string& name, BuildEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<BuildEnum>(
    BuildEnum_descriptor(), name, value);
}
enum RecordEnum {
  InvalidRecord = 0,
  CampRecord = 1,
  DailyRecord = 2,
  DeadRecord = 3,
  LifeRecord = 4,
  RecordEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  RecordEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool RecordEnum_IsValid(int value);
const RecordEnum RecordEnum_MIN = InvalidRecord;
const RecordEnum RecordEnum_MAX = LifeRecord;
const int RecordEnum_ARRAYSIZE = RecordEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* RecordEnum_descriptor();
inline const ::std::string& RecordEnum_Name(RecordEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    RecordEnum_descriptor(), value);
}
inline bool RecordEnum_Parse(
    const ::std::string& name, RecordEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<RecordEnum>(
    RecordEnum_descriptor(), name, value);
}
enum LifeEventEnum {
  HeroBorn = 0,
  HeroDead = 1,
  LifeEventEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  LifeEventEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool LifeEventEnum_IsValid(int value);
const LifeEventEnum LifeEventEnum_MIN = HeroBorn;
const LifeEventEnum LifeEventEnum_MAX = HeroDead;
const int LifeEventEnum_ARRAYSIZE = LifeEventEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* LifeEventEnum_descriptor();
inline const ::std::string& LifeEventEnum_Name(LifeEventEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    LifeEventEnum_descriptor(), value);
}
inline bool LifeEventEnum_Parse(
    const ::std::string& name, LifeEventEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<LifeEventEnum>(
    LifeEventEnum_descriptor(), name, value);
}
enum PlayerStatusEnumEx {
  UnknowStatus = 0,
  OnlineStatus = 1,
  OfflineStatus = 2,
  ExploreStatus = 3,
  PVEStatus = 4,
  PVPStatus = 5,
  TownStatus = 6,
  DropSelectStatus = 100,
  PlayerStatusEnumEx_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  PlayerStatusEnumEx_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool PlayerStatusEnumEx_IsValid(int value);
const PlayerStatusEnumEx PlayerStatusEnumEx_MIN = UnknowStatus;
const PlayerStatusEnumEx PlayerStatusEnumEx_MAX = DropSelectStatus;
const int PlayerStatusEnumEx_ARRAYSIZE = PlayerStatusEnumEx_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* PlayerStatusEnumEx_descriptor();
inline const ::std::string& PlayerStatusEnumEx_Name(PlayerStatusEnumEx value) {
  return ::google::protobuf::internal::NameOfEnum(
    PlayerStatusEnumEx_descriptor(), value);
}
inline bool PlayerStatusEnumEx_Parse(
    const ::std::string& name, PlayerStatusEnumEx* value) {
  return ::google::protobuf::internal::ParseNamedEnum<PlayerStatusEnumEx>(
    PlayerStatusEnumEx_descriptor(), name, value);
}
enum NoticeEnumEx {
  UnknowNotice = 0,
  NoticeCampRecord = 100,
  NoticeDailyRecord = 101,
  NoticeDeadRecord = 102,
  NoticeEnumEx_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  NoticeEnumEx_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool NoticeEnumEx_IsValid(int value);
const NoticeEnumEx NoticeEnumEx_MIN = UnknowNotice;
const NoticeEnumEx NoticeEnumEx_MAX = NoticeDeadRecord;
const int NoticeEnumEx_ARRAYSIZE = NoticeEnumEx_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* NoticeEnumEx_descriptor();
inline const ::std::string& NoticeEnumEx_Name(NoticeEnumEx value) {
  return ::google::protobuf::internal::NameOfEnum(
    NoticeEnumEx_descriptor(), value);
}
inline bool NoticeEnumEx_Parse(
    const ::std::string& name, NoticeEnumEx* value) {
  return ::google::protobuf::internal::ParseNamedEnum<NoticeEnumEx>(
    NoticeEnumEx_descriptor(), name, value);
}
enum ExpireEnum {
  InvalidExpire = 0,
  TimeExpire = 1,
  NumExpire = 2,
  ExpireEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  ExpireEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool ExpireEnum_IsValid(int value);
const ExpireEnum ExpireEnum_MIN = InvalidExpire;
const ExpireEnum ExpireEnum_MAX = NumExpire;
const int ExpireEnum_ARRAYSIZE = ExpireEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* ExpireEnum_descriptor();
inline const ::std::string& ExpireEnum_Name(ExpireEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    ExpireEnum_descriptor(), value);
}
inline bool ExpireEnum_Parse(
    const ::std::string& name, ExpireEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ExpireEnum>(
    ExpireEnum_descriptor(), name, value);
}
enum BalanceShowEnum {
  UnknowShow = 0,
  Explore = 1,
  Balance = 2,
  BalanceShowEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  BalanceShowEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool BalanceShowEnum_IsValid(int value);
const BalanceShowEnum BalanceShowEnum_MIN = UnknowShow;
const BalanceShowEnum BalanceShowEnum_MAX = Balance;
const int BalanceShowEnum_ARRAYSIZE = BalanceShowEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* BalanceShowEnum_descriptor();
inline const ::std::string& BalanceShowEnum_Name(BalanceShowEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    BalanceShowEnum_descriptor(), value);
}
inline bool BalanceShowEnum_Parse(
    const ::std::string& name, BalanceShowEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<BalanceShowEnum>(
    BalanceShowEnum_descriptor(), name, value);
}
enum ItemAutoEnum {
  AutoInvalid = 0,
  AutoStore = 1,
  AutoDestory = 2,
  ItemAutoEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  ItemAutoEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool ItemAutoEnum_IsValid(int value);
const ItemAutoEnum ItemAutoEnum_MIN = AutoInvalid;
const ItemAutoEnum ItemAutoEnum_MAX = AutoDestory;
const int ItemAutoEnum_ARRAYSIZE = ItemAutoEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* ItemAutoEnum_descriptor();
inline const ::std::string& ItemAutoEnum_Name(ItemAutoEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    ItemAutoEnum_descriptor(), value);
}
inline bool ItemAutoEnum_Parse(
    const ::std::string& name, ItemAutoEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ItemAutoEnum>(
    ItemAutoEnum_descriptor(), name, value);
}
enum InitialProcessEnum {
  ProcessInvalid = 0,
  ProcessCG = 1,
  ProcessPVE = 2,
  ProcessExplore = 3,
  ProcessChapter = 4,
  ProcessTask = 5,
  ProcessScene = 6,
  UIDialogue = 7,
  BubbleDialogue = 8,
  ProcessSequence = 9,
  ChapterAndStory = 10,
  ProcessStory = 11,
  CreateRole = 12,
  InitialProcessEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  InitialProcessEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool InitialProcessEnum_IsValid(int value);
const InitialProcessEnum InitialProcessEnum_MIN = ProcessInvalid;
const InitialProcessEnum InitialProcessEnum_MAX = CreateRole;
const int InitialProcessEnum_ARRAYSIZE = InitialProcessEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* InitialProcessEnum_descriptor();
inline const ::std::string& InitialProcessEnum_Name(InitialProcessEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    InitialProcessEnum_descriptor(), value);
}
inline bool InitialProcessEnum_Parse(
    const ::std::string& name, InitialProcessEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<InitialProcessEnum>(
    InitialProcessEnum_descriptor(), name, value);
}
enum RealmEnterEnum {
  EnterInvalid = 0,
  EnterChapter = 1,
  EnterLogin = 2,
  EnterTown = 3,
  EnterJump = 4,
  EnterExtend = 5,
  RealmEnterEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  RealmEnterEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool RealmEnterEnum_IsValid(int value);
const RealmEnterEnum RealmEnterEnum_MIN = EnterInvalid;
const RealmEnterEnum RealmEnterEnum_MAX = EnterExtend;
const int RealmEnterEnum_ARRAYSIZE = RealmEnterEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* RealmEnterEnum_descriptor();
inline const ::std::string& RealmEnterEnum_Name(RealmEnterEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    RealmEnterEnum_descriptor(), value);
}
inline bool RealmEnterEnum_Parse(
    const ::std::string& name, RealmEnterEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<RealmEnterEnum>(
    RealmEnterEnum_descriptor(), name, value);
}
enum ExtendLevelEnum {
  ExtendLevelInvalid = 0,
  ExtendLevel = 10000,
  ExtendLevelEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  ExtendLevelEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool ExtendLevelEnum_IsValid(int value);
const ExtendLevelEnum ExtendLevelEnum_MIN = ExtendLevelInvalid;
const ExtendLevelEnum ExtendLevelEnum_MAX = ExtendLevel;
const int ExtendLevelEnum_ARRAYSIZE = ExtendLevelEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* ExtendLevelEnum_descriptor();
inline const ::std::string& ExtendLevelEnum_Name(ExtendLevelEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    ExtendLevelEnum_descriptor(), value);
}
inline bool ExtendLevelEnum_Parse(
    const ::std::string& name, ExtendLevelEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ExtendLevelEnum>(
    ExtendLevelEnum_descriptor(), name, value);
}
enum BalanceEnum {
  UnknowBalance = 0,
  Victory = 1,
  Failed = 2,
  Flee = 3,
  Town = 4,
  Ace = 5,
  Chapter = 100,
  BalanceEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  BalanceEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool BalanceEnum_IsValid(int value);
const BalanceEnum BalanceEnum_MIN = UnknowBalance;
const BalanceEnum BalanceEnum_MAX = Chapter;
const int BalanceEnum_ARRAYSIZE = BalanceEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* BalanceEnum_descriptor();
inline const ::std::string& BalanceEnum_Name(BalanceEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    BalanceEnum_descriptor(), value);
}
inline bool BalanceEnum_Parse(
    const ::std::string& name, BalanceEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<BalanceEnum>(
    BalanceEnum_descriptor(), name, value);
}
enum JumpEnum {
  JumpInvalid = 0,
  Leave = 1,
  Enter = 2,
  JumpEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  JumpEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool JumpEnum_IsValid(int value);
const JumpEnum JumpEnum_MIN = JumpInvalid;
const JumpEnum JumpEnum_MAX = Enter;
const int JumpEnum_ARRAYSIZE = JumpEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* JumpEnum_descriptor();
inline const ::std::string& JumpEnum_Name(JumpEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    JumpEnum_descriptor(), value);
}
inline bool JumpEnum_Parse(
    const ::std::string& name, JumpEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<JumpEnum>(
    JumpEnum_descriptor(), name, value);
}
enum RealmTimeEnum {
  TimeInvalid = 0,
  Morning = 1,
  Afternoon = 2,
  Night = 3,
  RealmTimeEnum_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  RealmTimeEnum_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
LIBPROTOC_EXPORT bool RealmTimeEnum_IsValid(int value);
const RealmTimeEnum RealmTimeEnum_MIN = TimeInvalid;
const RealmTimeEnum RealmTimeEnum_MAX = Night;
const int RealmTimeEnum_ARRAYSIZE = RealmTimeEnum_MAX + 1;

LIBPROTOC_EXPORT const ::google::protobuf::EnumDescriptor* RealmTimeEnum_descriptor();
inline const ::std::string& RealmTimeEnum_Name(RealmTimeEnum value) {
  return ::google::protobuf::internal::NameOfEnum(
    RealmTimeEnum_descriptor(), value);
}
inline bool RealmTimeEnum_Parse(
    const ::std::string& name, RealmTimeEnum* value) {
  return ::google::protobuf::internal::ParseNamedEnum<RealmTimeEnum>(
    RealmTimeEnum_descriptor(), name, value);
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

template <> struct is_proto_enum< ::KFMsg::ConstDefineEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::ConstDefineEnum>() {
  return ::KFMsg::ConstDefineEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::CampEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::CampEnum>() {
  return ::KFMsg::CampEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::DivisorEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::DivisorEnum>() {
  return ::KFMsg::DivisorEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::HeroLockEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::HeroLockEnum>() {
  return ::KFMsg::HeroLockEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::HeroRemoveEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::HeroRemoveEnum>() {
  return ::KFMsg::HeroRemoveEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::BuildEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::BuildEnum>() {
  return ::KFMsg::BuildEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::RecordEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::RecordEnum>() {
  return ::KFMsg::RecordEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::LifeEventEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::LifeEventEnum>() {
  return ::KFMsg::LifeEventEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::PlayerStatusEnumEx> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::PlayerStatusEnumEx>() {
  return ::KFMsg::PlayerStatusEnumEx_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::NoticeEnumEx> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::NoticeEnumEx>() {
  return ::KFMsg::NoticeEnumEx_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::ExpireEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::ExpireEnum>() {
  return ::KFMsg::ExpireEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::BalanceShowEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::BalanceShowEnum>() {
  return ::KFMsg::BalanceShowEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::ItemAutoEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::ItemAutoEnum>() {
  return ::KFMsg::ItemAutoEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::InitialProcessEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::InitialProcessEnum>() {
  return ::KFMsg::InitialProcessEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::RealmEnterEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::RealmEnterEnum>() {
  return ::KFMsg::RealmEnterEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::ExtendLevelEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::ExtendLevelEnum>() {
  return ::KFMsg::ExtendLevelEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::BalanceEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::BalanceEnum>() {
  return ::KFMsg::BalanceEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::JumpEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::JumpEnum>() {
  return ::KFMsg::JumpEnum_descriptor();
}
template <> struct is_proto_enum< ::KFMsg::RealmTimeEnum> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::KFMsg::RealmTimeEnum>() {
  return ::KFMsg::RealmTimeEnum_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)


#ifdef _MSC_VER
	#pragma warning(  pop  )
#endif
#endif  // PROTOBUF_INCLUDED_EnumMessage_2eproto
