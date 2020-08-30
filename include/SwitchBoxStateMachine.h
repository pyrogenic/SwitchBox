#pragma once

// https://github.com/kekyo/gcc-toolchain/issues/3#issuecomment-356762548
#undef min
#undef max
#undef LITTLE_ENDIAN

#include <FunctionFSM.h>
#include <map>
#include <string>

enum Trigger {
  kTriggerToggleInput,
  kTriggerSelectInputA,
  kTriggerSelectInputB,

  kTriggerToggleValhallaPreamp,
  kTriggerSelectValhallaPreampBypass,
  kTriggerSelectValhallaPreampEngage,

  kTriggerToggleOutput,
  kTriggerSelectOutputA,
  kTriggerSelectOutputB,
  kTriggerSelectOutputC,

  kTriggerValhallaOutputLocked,
  kTriggerValhallaOutputUnlocked
};

typedef struct {
  std::string inputLabel;
} SwitchBoxState;

extern std::map<FunctionFsm *, const std::string> stateMachineNames;
extern std::map<FunctionState *, const std::string> stateNames;
extern std::map<Trigger, const std::string> triggerNames;
extern SwitchBoxState publicState;
const std::string &sbsm_trigger_name(const Trigger event);
void sbsm_setup();
void sbsm_loop();
void sbsm_trigger(Trigger state);
