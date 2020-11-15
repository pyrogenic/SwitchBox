#pragma once

// https://github.com/kekyo/gcc-toolchain/issues/3#issuecomment-356762548
#undef min
#undef max
#undef LITTLE_ENDIAN

#include <map>
#include <string>

#include "Fsm.h"

#define AUX_IN 0

// Amp bypass has impedance control, allowing output to the subwoofer without going through a preamp.
#define LEVEL_MATCHED_MONITOR 1

enum Trigger {
  kTriggerToggleInput,
  kTriggerSelectInputDigital,
  kTriggerSelectInputAnalog,
#if AUX_INPUT
  kTriggerSelectInputAuxiliary,
#endif
  kTriggerToggleSubwoofer,
  kTriggerSubwooferBypass,
  kTriggerSubwooferEngage,

  kTriggerToggleOutput,
  kTriggerSelectOutputGeshelli,
  kTriggerSelectOutputValhalla,
  kTriggerSelectOutputMonitor,

  kInteractiveTriggerCount,

  kTriggerPreampEngaged,
  kTriggerPreampBypassed,
  kTriggerMonitorEngaged,
  kTriggerMonitorBypassed,

  kTriggerToggleMenu,
  kTriggerMenuNone,
  kTriggerMenuQuick,
  kTriggerMenuInput,
  kTriggerMenuOutput,
  kTriggerMenuPower,
};

extern std::map<Fsm *, const std::string> stateMachineNames;
extern std::map<const State *, const std::string> stateNames;
extern std::map<Trigger, const std::string> triggerNames;

const std::string &sbsm_trigger_name(const Trigger event);
const std::string &sbsm_input_label();
const std::string &sbsm_subwoofer_label();
const std::string &sbsm_output_label();

void sbsm_setup();
void sbsm_loop();
void sbsm_trigger(Trigger state);
