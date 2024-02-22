//
// Created by anton on 21.03.23.
//
#include "module.h"
#include "SwitcherComponent.h"
#include "MixerComponent.h"

DEFINE_MODULE(MixerModule)

REGISTER_MODULE_COMPONENT(MixerModule, MixerSwitchComponent)
REGISTER_MODULE_COMPONENT(MixerModule, MixerComponent)

SET_MODULE_PROVIDED_PROFILE(MixerModule, CREATE_PROFILE( SET_BUTTON(0, "MixerComponent"),
                                                         SET_BUTTON(1, "MixerComponent"),
                                                         SET_BUTTON(2, "MixerComponent"),
                                                         SET_BUTTON(3, "MixerComponent"),
                                                         SET_BUTTON(4, "MixerComponent"),
                                                         SET_BUTTON(5, "MixerComponent"),
                                                         SET_BUTTON(6, "MixerComponent"),
                                                         SET_BUTTON(7, "MixerComponent"),
                                                         SET_BUTTON(8, "MixerComponent"),
                                                         SET_BUTTON(9, "MixerComponent"),
                                                         SET_BUTTON(10, "MixerComponent"),
                                                         SET_BUTTON(11, "MixerComponent"),
                                                         SET_BUTTON(12, "MixerComponent"),
                                                         SET_BUTTON(13, "MixerComponent"),
                                                         SET_BUTTON(14, "MixerComponent")
))

