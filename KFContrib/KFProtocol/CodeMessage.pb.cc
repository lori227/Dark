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
      "\n\021CodeMessage.proto\022\005KFMsg*\230\034\n\017ProjectCo"
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
      "tEnough\020\241\363\001\022\033\n\025HeroRecruitRefreshSuc\020\242\363\001"
      "\022\030\n\022HeroRecruitHeroSuc\020\243\363\001\022\031\n\023TrainCampL"
      "evelError\020\340\363\001\022\031\n\023TrainCampIndexError\020\341\363\001"
      "\022\033\n\025TrainCampHeroNotExist\020\342\363\001\022\032\n\024TrainCa"
      "mpHeroIsExist\020\343\363\001\022\030\n\022BuildFuncNotActive\020"
      "\344\363\001\022\032\n\024MainCampSkinNotExist\020\345\363\001\022\033\n\025MainC"
      "ampSkinNotActive\020\346\363\001\022\030\n\022ClinicSettingErr"
      "or\020\347\363\001\022\033\n\025ClinicItemIsNotEnough\020\350\363\001\022\030\n\022C"
      "linicMoneyIsError\020\351\363\001\022\033\n\025TrainCampSettin"
      "gError\020\352\363\001\022\031\n\023TrainCampIsFinished\020\353\363\001\022\031\n"
      "\023GranarySettingError\020\354\363\001\022\030\n\022GranaryHaveN"
      "otItem\020\355\363\001\022\034\n\026SmithyConsumeNotEnough\020\356\363\001"
      "\022\030\n\022SmithySettingError\020\357\363\001\022\027\n\021SmithyHave"
      "NotItem\020\360\363\001\022\026\n\020SmithyStoreIsMax\020\361\363\001\022\036\n\030S"
      "mithyWeaponSettingError\020\362\363\001\022\030\n\022SmithyMak"
      "eNumError\020\363\363\001\022\034\n\026SmithyMakeNumNotEnough\020"
      "\364\363\001\022\034\n\026ClinicCountIsNotEnough\020\365\363\001\022\027\n\021Cli"
      "nicNotNeedCure\020\366\363\001\022\031\n\023SmithyMakeWeaponSu"
      "c\020\367\363\001\022\025\n\017BuildLevelIsMax\020\370\363\001\022\030\n\022BuildInU"
      "pgradeTime\020\371\363\001\022\034\n\026BuildNoFreeUpgradeList"
      "\020\372\363\001\022\037\n\031BuildUpgradeLackCondition\020\373\363\001\022\033\n"
      "\025BuildNotInUpgradeTime\020\374\363\001\022\034\n\026BuildLevel"
      "SettingError\020\375\363\001\022\034\n\026TechnologySettingErr"
      "or\020\376\363\001\022 \n\032TechnologyLackPreCondition\020\377\363\001"
      "\022\030\n\022TechnologyIsUnlock\020\200\364\001\022\026\n\020Technology"
      "Islock\020\201\364\001\022\025\n\017BuildUpgradeSuc\020\202\364\001\022\024\n\016Tra"
      "inNotFinish\020\203\364\001\022\032\n\024SmithyMakeLevelLimit\020"
      "\204\364\001\022\027\n\021FormulaParamError\020\205\364\001\022\032\n\024GranaryB"
      "uyCountLimit\020\206\364\001\022\034\n\026GranaryBuySettingErr"
      "or\020\207\364\001\022\032\n\024GranaryBuyParamError\020\210\364\001\022\023\n\rRe"
      "almMapError\020\305\364\001\022\025\n\017RealmLevelError\020\306\364\001\022\027"
      "\n\021RealmNotInExplore\020\311\364\001\022\020\n\nPVEIdError\020\312\364"
      "\001\022\020\n\nPVEAlready\020\313\364\001\022\022\n\014RealmAlready\020\314\364\001\022"
      "\026\n\020RealmNotInStatus\020\315\364\001\022\024\n\016PVENotInStatu"
      "s\020\316\364\001\022\026\n\020PVEHeroTeamEmpty\020\317\364\001\022\026\n\020PVEHero"
      "TeamExist\020\320\364\001\022\023\n\rPVEMapIdError\020\321\364\001\022\024\n\016Re"
      "almDataError\020\322\364\001\022\025\n\017RealmIdMismatch\020\323\364\001\022"
      "\022\n\014RealmNotTown\020\324\364\001\022\021\n\013RealmInTown\020\325\364\001\022\030"
      "\n\022RealmHeroTeamEmpty\020\326\364\001\022\024\n\016ChapterIdErr"
      "or\020\221\375\001\022\024\n\016ChapterNotOpen\020\222\375\001\022\027\n\021ChapterE"
      "ventError\020\223\375\001\022\031\n\023ChapterDefaultError\020\224\375\001"
      "\022\032\n\024ChapterStatusIdError\020\225\375\001\022\032\n\024ChapterE"
      "xecuteFailed\020\226\375\001\022\024\n\016ItemWeaponOnly\020\364\375\001\022\031"
      "\n\023ItemWeaponRaceLimit\020\365\375\001\022\031\n\023ItemWeaponH"
      "eroLimit\020\366\375\001\022\032\n\024ItemWeaponLevelLimit\020\367\375\001"
      "\022\026\n\020ItemWeaponFailed\020\370\375\001\022\022\n\014ItemWeaponOk"
      "\020\371\375\001\022\030\n\022ItemWeaponNotExist\020\372\375\001\022\030\n\022ItemUn"
      "WeaponFailed\020\373\375\001\022\024\n\016ItemUnWeaponOk\020\374\375\001\022\037"
      "\n\031ItemUpdateDurabilityError\020\375\375\001\022\031\n\023Playe"
      "rHeadIconSetOK\020\331\376\001\022\030\n\022PlayerFactionSetOK"
      "\020\332\376\001\022\034\n\026PlayerHeadIconNotExist\020\333\376\001\022\033\n\025Pl"
      "ayerFactionNotExist\020\334\376\001\022\032\n\024PlayerHeadIco"
      "nNotHad\020\335\376\001\022\032\n\024StorySettingNotExist\020\274\377\001\022"
      "\023\n\rStoryNotExist\020\275\377\001\022\032\n\024StorySequenceIsE"
      "rror\020\276\377\001\022\033\n\025StoryTriggerTypeLimit\020\277\377\001\022\035\n"
      "\027DialogueSettingNotExist\020\300\377\001\022\026\n\020Dialogue"
      "NotExist\020\301\377\001\022\033\n\025DialogueHaveNotBranch\020\302\377"
      "\001\022\034\n\026DialogueBranchNotExist\020\303\377\001\022!\n\033Dialo"
      "gueBranchLackCondition\020\304\377\001\022\033\n\025BranchSett"
      "ingNotExist\020\305\377\001\022\035\n\027DialogueBranchSelectS"
      "uc\020\306\377\001\022\035\n\027StoryHaveDialogueBranch\020\307\377\001b\006p"
      "roto3"
  };
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
      descriptor, 3645);
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
    case 31138:
    case 31139:
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
    case 31234:
    case 31235:
    case 31236:
    case 31237:
    case 31238:
    case 31239:
    case 31240:
    case 31301:
    case 31302:
    case 31305:
    case 31306:
    case 31307:
    case 31308:
    case 31309:
    case 31310:
    case 31311:
    case 31312:
    case 31313:
    case 31314:
    case 31315:
    case 31316:
    case 31317:
    case 31318:
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
    case 32601:
    case 32602:
    case 32603:
    case 32604:
    case 32605:
    case 32700:
    case 32701:
    case 32702:
    case 32703:
    case 32704:
    case 32705:
    case 32706:
    case 32707:
    case 32708:
    case 32709:
    case 32710:
    case 32711:
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
