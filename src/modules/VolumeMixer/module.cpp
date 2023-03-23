//
// Created by anton on 21.03.23.
//
#include "module.h"
#include "SwitcherComponent.h"

DEFINE_MODULE(MixerModule)

REGISTER_MODULE_COMPONENT(MixerModule, MixerSwitchComponent)

SET_MODULE_PROVIDED_PROFILE(MixerModule, CREATE_PROFILE( SET_BUTTON(1, "MixerSwitchComponent"),
                                                         SET_BUTTON(2, "2"),
                                                         SET_BUTTON(3, "2"),
                                                         SET_BUTTON(4, "2"),
                                                         SET_BUTTON(5, "2")
))

