// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: CodeMessage.proto

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable : 4125)
	#pragma warning(disable : 4668)
	#pragma warning(disable : 4800)
	#pragma warning(disable : 4647)
#endif

#include "CodeMessage.pb.h"

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
namespace protobuf_CodeMessage_2eproto {
void InitDefaults() {
}

const ::google::protobuf::EnumDescriptor* file_level_enum_descriptors[1];
const ::google::protobuf::uint32 TableStruct::offsets[1] = {};
static const ::google::protobuf::internal::MigrationSchema* schemas = NULL;
static const ::google::protobuf::Message* const* file_default_instances = NULL;

void protobuf_AssignDescriptors() {
  AddDescriptors();
  AssignDescriptors(
      "CodeMessage.proto", schemas, file_default_instances, TableStruct::offsets,
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
      "\n\021CodeMessage.proto\022\005KFMsg*\264\025\n\017ProjectCo"
      "deEnum\022\016\n\nLogicError\020\000\022\024\n\016MatchRequestOk"
      "\020\231\362\001\022\026\n\020MatchAlreadyWait\020\232\362\001\022\021\n\013MatchInR"
      "oom\020\233\362\001\022\025\n\017MatchNotInMatch\020\234\362\001\022\023\n\rMatchC"
      "ancelOk\020\235\362\001\022\030\n\022MatchAffirmTimeout\020\237\362\001\022\025\n"
      "\017RoomAllotBattle\020\240\362\001\022\022\n\014MatchIdError\020\241\362\001"
      "\022\026\n\020HeroDivisorError\020\375\362\001\022\026\n\020HeroDivisorE"
      "xist\020\376\362\001\022\030\n\022HeroRecruitRefresh\020\377\362\001\022!\n\033He"
      "roRecruitRefreshFreeError\020\200\363\001\022!\n\033HeroRec"
      "ruitRefreshTypeError\020\201\363\001\022\031\n\023HeroRecruitN"
      "otExist\020\202\363\001\022\032\n\024HeroRecruitCostError\020\203\363\001\022"
      "\032\n\024HeroRecruitDataError\020\204\363\001\022\023\n\rHeroRecru"
      "itOk\020\205\363\001\022\031\n\023HeroDivisorMaxCount\020\206\363\001\022\022\n\014H"
      "eroNotExist\020\207\363\001\022\030\n\022HeroTeamIndexError\020\210\363"
      "\001\022\031\n\023HeroTeamDataInvalid\020\211\363\001\022\027\n\021HeroNotI"
      "nHeroList\020\212\363\001\022\024\n\016HeroLevelIsMax\020\213\363\001\022\027\n\021H"
      "eroLockNotChange\020\214\363\001\022\022\n\014HeroIsLocked\020\215\363\001"
      "\022\027\n\021HeroLockFlagError\020\216\363\001\022\027\n\021HeroNameNot"
      "Change\020\217\363\001\022\030\n\022HeroSkillNotChange\020\220\363\001\022\031\n\023"
      "HeroSkillIndexError\020\221\363\001\022\021\n\013HeroHpIsMax\020\222"
      "\363\001\022\033\n\025HeroTransferDataError\020\223\363\001\022\030\n\022HeroL"
      "evelNotEnough\020\224\363\001\022\033\n\025HeroTransferCostErr"
      "or\020\225\363\001\022\030\n\022HeroCanNotTransfer\020\226\363\001\022\033\n\025Hero"
      "ProfessionIsError\020\227\363\001\022\023\n\rHeroNameEmpty\020\230"
      "\363\001\022\025\n\017HeroTransferSuc\020\231\363\001\022\031\n\023HeroSelectI"
      "nnateSuc\020\232\363\001\022\020\n\nHeroIsDead\020\233\363\001\022\025\n\017HeroRa"
      "ceIsError\020\234\363\001\022\032\n\024HeroDivisorNotUnlock\020\235\363"
      "\001\022\035\n\027HeroRecruitRefreshCount\020\236\363\001\022 \n\032Hero"
      "RecruitRefreshGenerate\020\237\363\001\022\036\n\030HeroRecrui"
      "tRefreshWeight\020\240\363\001\022\037\n\031HeroRefreshCountNo"
      "tEnough\020\241\363\001\022\031\n\023TrainCampLevelError\020\340\363\001\022\031"
      "\n\023TrainCampIndexError\020\341\363\001\022\033\n\025TrainCampHe"
      "roNotExist\020\342\363\001\022\032\n\024TrainCampHeroIsExist\020\343"
      "\363\001\022\030\n\022BuildFuncNotActive\020\344\363\001\022\032\n\024MainCamp"
      "SkinNotExist\020\345\363\001\022\033\n\025MainCampSkinNotActiv"
      "e\020\346\363\001\022\030\n\022ClinicSettingError\020\347\363\001\022\033\n\025Clini"
      "cItemIsNotEnough\020\350\363\001\022\030\n\022ClinicMoneyIsErr"
      "or\020\351\363\001\022\033\n\025TrainCampSettingError\020\352\363\001\022\031\n\023T"
      "rainCampIsFinished\020\353\363\001\022\031\n\023GranarySetting"
      "Error\020\354\363\001\022\030\n\022GranaryHaveNotItem\020\355\363\001\022\034\n\026S"
      "mithyConsumeNotEnough\020\356\363\001\022\030\n\022SmithySetti"
      "ngError\020\357\363\001\022\027\n\021SmithyHaveNotItem\020\360\363\001\022\026\n\020"
      "SmithyStoreIsMax\020\361\363\001\022\036\n\030SmithyWeaponSett"
      "ingError\020\362\363\001\022\030\n\022SmithyMakeNumError\020\363\363\001\022\034"
      "\n\026SmithyMakeNumNotEnough\020\364\363\001\022\034\n\026ClinicCo"
      "untIsNotEnough\020\365\363\001\022\027\n\021ClinicNotNeedCure\020"
      "\366\363\001\022\031\n\023SmithyMakeWeaponSuc\020\367\363\001\022\025\n\017BuildL"
      "evelIsMax\020\370\363\001\022\030\n\022BuildInUpgradeTime\020\371\363\001\022"
      "\034\n\026BuildNoFreeUpgradeList\020\372\363\001\022\037\n\031BuildUp"
      "gradeLackCondition\020\373\363\001\022\033\n\025BuildNotInUpgr"
      "adeTime\020\374\363\001\022\034\n\026BuildLevelSettingError\020\375\363"
      "\001\022\034\n\026TechnologySettingError\020\376\363\001\022 \n\032Techn"
      "ologyLackPreCondition\020\377\363\001\022\030\n\022TechnologyI"
      "sUnlock\020\200\364\001\022\026\n\020TechnologyIslock\020\201\364\001\022\025\n\017E"
      "xploreMapLimit\020\307\364\001\022\025\n\017ExploreMapError\020\310\364"
      "\001\022\031\n\023ExploreNotInExplore\020\311\364\001\022\020\n\nPVEIdErr"
      "or\020\312\364\001\022\020\n\nPVEAlready\020\313\364\001\022\024\n\016ExploreAlrea"
      "dy\020\314\364\001\022\030\n\022ExploreNotInStatus\020\315\364\001\022\024\n\016PVEN"
      "otInStatus\020\316\364\001\022\032\n\024ExploreHeroTeamEmpty\020\317"
      "\364\001\022\024\n\016ChapterIdError\020\221\375\001\022\024\n\016ChapterNotOp"
      "en\020\222\375\001\022\027\n\021ChapterEventError\020\223\375\001\022\031\n\023Chapt"
      "erDefaultError\020\224\375\001\022\032\n\024ChapterStatusIdErr"
      "or\020\225\375\001\022\032\n\024ChapterExecuteFailed\020\226\375\001\022\024\n\016It"
      "emWeaponOnly\020\364\375\001\022\031\n\023ItemWeaponRaceLimit\020"
      "\365\375\001\022\031\n\023ItemWeaponHeroLimit\020\366\375\001\022\032\n\024ItemWe"
      "aponLevelLimit\020\367\375\001\022\026\n\020ItemWeaponFailed\020\370"
      "\375\001\022\022\n\014ItemWeaponOk\020\371\375\001\022\030\n\022ItemWeaponNotE"
      "xist\020\372\375\001\022\030\n\022ItemUnWeaponFailed\020\373\375\001\022\024\n\016It"
      "emUnWeaponOk\020\374\375\001\022\037\n\031ItemUpdateDurability"
      "Error\020\375\375\001b\006proto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 2777);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "CodeMessage.proto", &protobuf_RegisterTypes);
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
}  // namespace protobuf_CodeMessage_2eproto
namespace KFMsg {
const ::google::protobuf::EnumDescriptor* ProjectCodeEnum_descriptor() {
  protobuf_CodeMessage_2eproto::protobuf_AssignDescriptorsOnce();
  return protobuf_CodeMessage_2eproto::file_level_enum_descriptors[0];
}
bool ProjectCodeEnum_IsValid(int value) {
  switch (value) {
    case 0:
    case 31001:
    case 31002:
    case 31003:
    case 31004:
    case 31005:
    case 31007:
    case 31008:
    case 31009:
    case 31101:
    case 31102:
    case 31103:
    case 31104:
    case 31105:
    case 31106:
    case 31107:
    case 31108:
    case 31109:
    case 31110:
    case 31111:
    case 31112:
    case 31113:
    case 31114:
    case 31115:
    case 31116:
    case 31117:
    case 31118:
    case 31119:
    case 31120:
    case 31121:
    case 31122:
    case 31123:
    case 31124:
    case 31125:
    case 31126:
    case 31127:
    case 31128:
    case 31129:
    case 31130:
    case 31131:
    case 31132:
    case 31133:
    case 31134:
    case 31135:
    case 31136:
    case 31137:
    case 31200:
    case 31201:
    case 31202:
    case 31203:
    case 31204:
    case 31205:
    case 31206:
    case 31207:
    case 31208:
    case 31209:
    case 31210:
    case 31211:
    case 31212:
    case 31213:
    case 31214:
    case 31215:
    case 31216:
    case 31217:
    case 31218:
    case 31219:
    case 31220:
    case 31221:
    case 31222:
    case 31223:
    case 31224:
    case 31225:
    case 31226:
    case 31227:
    case 31228:
    case 31229:
    case 31230:
    case 31231:
    case 31232:
    case 31233:
    case 31303:
    case 31304:
    case 31305:
    case 31306:
    case 31307:
    case 31308:
    case 31309:
    case 31310:
    case 31311:
    case 32401:
    case 32402:
    case 32403:
    case 32404:
    case 32405:
    case 32406:
    case 32500:
    case 32501:
    case 32502:
    case 32503:
    case 32504:
    case 32505:
    case 32506:
    case 32507:
    case 32508:
    case 32509:
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