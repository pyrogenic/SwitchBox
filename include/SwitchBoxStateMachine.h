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
  kTriggerSelectInputDigital,
  kTriggerSelectInputAnalog,

  kTriggerToggleSubwoofer,
  kTriggerSubwooferBypass,
  kTriggerSubwooferEngage,

  kTriggerToggleOutput,
  kTriggerSelectOutputGeshelli,
  kTriggerSelectOutputValhalla,

  kTriggerActivateMonitor,

  kInteractiveTriggerCount,

  kTriggerPreampEngaged,
  kTriggerPreampBypassed,
};

extern std::map<FunctionFsm *, const std::string> stateMachineNames;
extern std::map<FunctionState *, const std::string> stateNames;
extern std::map<Trigger, const std::string> triggerNames;

const std::string &sbsm_trigger_name(const Trigger event);
const std::string &sbsm_input_label();
const std::string &sbsm_subwoofer_label();
const std::string &sbsm_output_label();

void sbsm_setup();
void sbsm_loop();
void sbsm_trigger(Trigger state);
