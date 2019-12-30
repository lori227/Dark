// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: EnumMessage.proto

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4125)
	#pragma warning(disable : 4668)
	#pragma warning(disable : 4800)
	#pragma warning(disable : 4647)
#endif

#include "EnumMessage.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/port.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// This is a temporary google only hack
#ifdef GOOGLE_PROTOBUF_ENFORCE_UNIQUENESS
#include "third_party/protobuf/version.h"
#endif
// @@protoc_insertion_point(includes)

namespace KFMsg {
}  // namespace KFMsg
namespace protobuf_EnumMessage_2eproto {
void InitDefaults() {
}

const ::google::protobuf::EnumDescriptor* file_level_enum_descriptors[15];
const ::google::protobuf::uint32 TableStruct::offsets[1] = {};
static const ::google::protobuf::internal::MigrationSchema* schemas = NULL;
static const ::google::protobuf::Message* const* file_default_instances = NULL;

void protobuf_AssignDescriptors() {
  AddDescriptors();
  AssignDescriptors(
      "EnumMessage.proto", schemas, file_default_instances, TableStruct::offsets,
      NULL, file_level_enum_descriptors, NULL);
}

void protobuf_AssignDescriptorsOnce() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, protobuf_AssignDescriptors);
}

void protobuf_RegisterTypes(const ::std::string&) GOOGLE_PROTOBUF_ATTRIBUTE_COLD;
void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

void AddDescriptorsImpl() {
  InitDefaults();
  static const char descriptor[] GOOGLE_PROTOBUF_ATTRIBUTE_SECTION_VARIABLE(protodesc_cold) = {
      "\n\021EnumMessage.proto\022\005KFMsg*5\n\010CampEnum\022\016"
      "\n\nUnknowCamp\020\000\022\013\n\007RedCamp\020\001\022\014\n\010BlueCamp\020"
      "\002*Z\n\013RefreshEnum\022\021\n\rUnknowRefresh\020\000\022\021\n\rR"
      "efreshByFree\020\001\022\022\n\016RefreshByCount\020\002\022\021\n\rRe"
      "freshByCost\020\003*[\n\013DivisorEnum\022\021\n\rUnknowDi"
      "visor\020\000\022\021\n\rDivisorOfRace\020\001\022\021\n\rDivisorOfM"
      "ove\020\002\022\023\n\017DivisorOfWeapon\020\003*;\n\014HeroLockEn"
      "um\022\n\n\006NoLock\020\000\022\020\n\014HeroListLock\020\001\022\r\n\tCoun"
      "tLock\020\002*C\n\013BalanceEnum\022\021\n\rUnknowBalance\020"
      "\000\022\013\n\007Victory\020\001\022\n\n\006Failed\020\002\022\010\n\004Flee\020\003*\224\002\n"
      "\tBuildEnum\022\020\n\014InvalidBuild\020\000\022\r\n\010HeroList"
      "\020\310\001\022\r\n\010HeroTeam\020\311\001\022\016\n\tMainBuild\020\312\001\022\021\n\014Re"
      "cruitBuild\020\313\001\022\020\n\013ClinicBuild\020\314\001\022\017\n\nTrain"
      "Build\020\315\001\022\017\n\nOperaBuild\020\316\001\022\020\n\013SmithyBuild"
      "\020\317\001\022\016\n\tShopBuild\020\320\001\022\021\n\014InheritBuild\020\321\001\022\022"
      "\n\rCemeteryBuild\020\322\001\022\017\n\nOuterBuild\020\323\001\022\023\n\016W"
      "arehouseBuild\020\324\001\022\021\n\014GranaryBuild\020\325\001*`\n\nR"
      "ecordEnum\022\021\n\rInvalidRecord\020\000\022\016\n\nCampReco"
      "rd\020\001\022\017\n\013DailyRecord\020\002\022\016\n\nDeadRecord\020\003\022\016\n"
      "\nLifeRecord\020\004*+\n\rLifeEventEnum\022\014\n\010HeroBo"
      "rn\020\000\022\014\n\010HeroDead\020\001*|\n\022PlayerStatusEnumEx"
      "\022\020\n\014UnknowStatus\020\000\022\020\n\014OnlineStatus\020\001\022\021\n\r"
      "OfflineStatus\020\002\022\021\n\rExploreStatus\020\003\022\r\n\tPV"
      "EStatus\020\004\022\r\n\tPVPStatus\020\005*c\n\014NoticeEnumEx"
      "\022\020\n\014UnknowNotice\020\000\022\024\n\020NoticeCampRecord\020d"
      "\022\025\n\021NoticeDailyRecord\020e\022\024\n\020NoticeDeadRec"
      "ord\020f*>\n\nExpireEnum\022\021\n\rInvalidExpire\020\000\022\016"
      "\n\nTimeExpire\020\001\022\r\n\tNumExpire\020\002*;\n\017Balance"
      "ShowEnum\022\016\n\nUnknowShow\020\000\022\013\n\007Explore\020\001\022\013\n"
      "\007Balance\020\002*\?\n\014ItemAutoEnum\022\017\n\013AutoInvali"
      "d\020\000\022\r\n\tAutoStore\020\001\022\017\n\013AutoDestory\020\002*Y\n\017H"
      "eroDeathReason\022\021\n\rNoDeathReason\020\000\022\013\n\007NoE"
      "xist\020\001\022\016\n\nNoEnoughHp\020\002\022\026\n\022NoEnoughDurabi"
      "lity\020\003*\220\001\n\022InitialProcessEnum\022\022\n\016Process"
      "Invalid\020\000\022\r\n\tProcessCG\020\001\022\016\n\nProcessPVE\020\002"
      "\022\022\n\016ProcessExplore\020\003\022\020\n\014ProcessWorld\020\004\022\017"
      "\n\013ProcessTask\020\005\022\020\n\014ProcessScene\020\006b\006proto"
      "3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 1481);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "EnumMessage.proto", &protobuf_RegisterTypes);
}

void AddDescriptors() {
  static ::google::protobuf::internal::once_flag once;
  ::google::protobuf::internal::call_once(once, AddDescriptorsImpl);
}
// Force AddDescriptors() to be called at dynamic initialization time.
struct StaticDescriptorInitializer {
  StaticDescriptorInitializer() {
    AddDescriptors();
  }
} static_descriptor_initializer;
}  // namespace protobuf_EnumMessage_2eproto
namespace KFMsg {
const ::google::protobuf::EnumDescriptor* CampEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[0];
}
bool CampEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* RefreshEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[1];
}
bool RefreshEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* DivisorEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[2];
}
bool DivisorEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* HeroLockEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[3];
}
bool HeroLockEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* BalanceEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[4];
}
bool BalanceEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* BuildEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[5];
}
bool BuildEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 200:
    case 201:
    case 202:
    case 203:
    case 204:
    case 205:
    case 206:
    case 207:
    case 208:
    case 209:
    case 210:
    case 211:
    case 212:
    case 213:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* RecordEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[6];
}
bool RecordEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* LifeEventEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[7];
}
bool LifeEventEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* PlayerStatusEnumEx_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[8];
}
bool PlayerStatusEnumEx_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* NoticeEnumEx_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[9];
}
bool NoticeEnumEx_IsValid(int value) {
  switch (value) {
    case 0:
    case 100:
    case 101:
    case 102:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* ExpireEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[10];
}
bool ExpireEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* BalanceShowEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[11];
}
bool BalanceShowEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* ItemAutoEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[12];
}
bool ItemAutoEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* HeroDeathReason_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[13];
}
bool HeroDeathReason_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* InitialProcessEnum_descriptor() {
  protobuf_EnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_EnumMessage_2eproto::file_level_enum_descriptors[14];
}
bool InitialProcessEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace KFMsg
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google

#ifdef _MSC_VER
	#pragma warning(  pop  )
#endif

// @@protoc_insertion_point(global_scope)
