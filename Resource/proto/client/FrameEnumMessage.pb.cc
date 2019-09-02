// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: FrameEnumMessage.proto

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4125)
	#pragma warning(disable : 4668)
	#pragma warning(disable : 4800)
	#pragma warning(disable : 4647)
#endif

#include "FrameEnumMessage.pb.h"

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
namespace protobuf_FrameEnumMessage_2eproto {
void InitDefaults() {
}

const ::google::protobuf::EnumDescriptor* file_level_enum_descriptors[9];
const ::google::protobuf::uint32 TableStruct::offsets[1] = {};
static const ::google::protobuf::internal::MigrationSchema* schemas = NULL;
static const ::google::protobuf::Message* const* file_default_instances = NULL;

void protobuf_AssignDescriptors() {
  AddDescriptors();
  AssignDescriptors(
      "FrameEnumMessage.proto", schemas, file_default_instances, TableStruct::offsets,
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
      "\n\026FrameEnumMessage.proto\022\005KFMsg*E\n\013Chann"
      "elEnum\022\021\n\rUnknowChannel\020\000\022\014\n\010Internal\020\001\022"
      "\n\n\006WeiXin\020\002\022\t\n\005Steam\020\003*.\n\007SexEnum\022\r\n\tUnk"
      "nowSex\020\000\022\010\n\004Male\020\001\022\n\n\006Female\020\002*\?\n\010KickEn"
      "um\022\016\n\nUnknowKick\020\000\022\017\n\013KickByLogin\020\001\022\022\n\016K"
      "ickByPlatform\020\002*^\n\nStatusEnum\022\016\n\nInitSta"
      "tus\020\000\022\016\n\nDoneStatus\020\001\022\021\n\rReceiveStatus\020\002"
      "\022\n\n\006Remove\020\003\022\021\n\rReceiveRemove\020\004*\204\001\n\010UUID"
      "Enum\022\r\n\tUUidLogic\020\000\022\017\n\013UUidAccount\020\001\022\016\n\n"
      "UUidPlayer\020\002\022\014\n\010UUidItem\020\003\022\014\n\010UUidHero\020\004"
      "\022\r\n\tUUidGroup\020\005\022\r\n\tUUidGuild\020\006\022\016\n\nUUidBa"
      "ttle\020\007*:\n\010MailEnum\022\016\n\nUnknowMail\020\000\022\016\n\nGl"
      "obalMail\020\001\022\016\n\nPersonMail\020\002*7\n\010RankEnum\022\016"
      "\n\nUnknowRank\020\000\022\r\n\tTotalRank\020\001\022\014\n\010ZoneRan"
      "k\020\002*X\n\020PlayerStatusEnum\022\025\n\021UnknowFrameSt"
      "atus\020\000\022\025\n\021FrameOnlineStatus\020\001\022\026\n\022FrameOf"
      "flineStatus\020\002*U\n\nInviteEnum\022\020\n\014UnknowInv"
      "ite\020\000\022\013\n\007Consent\020\001\022\n\n\006Refuse\020\002\022\n\n\006Delete"
      "\020\003\022\020\n\014RefuseMinute\020\004b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 748);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "FrameEnumMessage.proto", &protobuf_RegisterTypes);
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
}  // namespace protobuf_FrameEnumMessage_2eproto
namespace KFMsg {
const ::google::protobuf::EnumDescriptor* ChannelEnum_descriptor() {
  protobuf_FrameEnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_FrameEnumMessage_2eproto::file_level_enum_descriptors[0];
}
bool ChannelEnum_IsValid(int value) {
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

const ::google::protobuf::EnumDescriptor* SexEnum_descriptor() {
  protobuf_FrameEnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_FrameEnumMessage_2eproto::file_level_enum_descriptors[1];
}
bool SexEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* KickEnum_descriptor() {
  protobuf_FrameEnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_FrameEnumMessage_2eproto::file_level_enum_descriptors[2];
}
bool KickEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* StatusEnum_descriptor() {
  protobuf_FrameEnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_FrameEnumMessage_2eproto::file_level_enum_descriptors[3];
}
bool StatusEnum_IsValid(int value) {
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

const ::google::protobuf::EnumDescriptor* UUIDEnum_descriptor() {
  protobuf_FrameEnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_FrameEnumMessage_2eproto::file_level_enum_descriptors[4];
}
bool UUIDEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* MailEnum_descriptor() {
  protobuf_FrameEnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_FrameEnumMessage_2eproto::file_level_enum_descriptors[5];
}
bool MailEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* RankEnum_descriptor() {
  protobuf_FrameEnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_FrameEnumMessage_2eproto::file_level_enum_descriptors[6];
}
bool RankEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* PlayerStatusEnum_descriptor() {
  protobuf_FrameEnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_FrameEnumMessage_2eproto::file_level_enum_descriptors[7];
}
bool PlayerStatusEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}

const ::google::protobuf::EnumDescriptor* InviteEnum_descriptor() {
  protobuf_FrameEnumMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_FrameEnumMessage_2eproto::file_level_enum_descriptors[8];
}
bool InviteEnum_IsValid(int value) {
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
