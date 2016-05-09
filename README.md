# MMT_Plugin
Machinery Modelling Toolkit is a plugin for UE4. This plugin provides some basic means to add custom physics code in blueprints, which can be executed during physics sub-stepping. This repository contains only plugin binary for x64 and source code.

The basis of the plugin is a custom pawn MMT_Pawn, which has a custom event "MMT_Physics_Tick" executed during normal and sub-stepped physics updates. In addition to MMT_Pawn you will find several functions which a necessary to query and interact with objects during physics sub-stepping. All functions have prefix "MMT" so it's easy to find them among blueprint nodes.

Installation:
Copy content of this repository into [your project main folder]/Plugins/MMT/
if you don't have Plugins folder just make one. Plugin will be automatically picked up by editor.

Instruction on how to use plugin on your own without content examples:
https://github.com/BoredEngineer/MMT_Plugin/wiki

More information can be found here:
https://forums.unrealengine.com/showthread.php?99605-OPEN-SOURCE-Machinery-Modelling-Toolkit
