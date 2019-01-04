# BakkesMod Curveball Plugin
Adds Magnus force and drag to the ball in Rocket League

## installation

Copy BendItPlugin.dll to <steam_directory>/steamapps/common/rocketleague/Binaries/Win32/bakkesmod/plugins
Copy benditplugin.set to <steam_directory>/steamapps/common/rocketleague/Binaries/Win32/bakkesmod/plugins/settings

## commands

**sv_soccar_curve 0|1**   -- turn the effect on and off

**sv_soccar_drag_coefficient \<float\>**  -- set the drag coefficient , this is 1 of two parameters that effect the ball flight

**sv_soccar_lift_coefficient \<float\>** -- affects how much the curve turns the ball

**sv_soccar_lift_zmod \<float\>** -- multiplier for the vertical curving

**sv_soccar_maxspin \<float\>** -- multiplier for the maximum spin speed the ball can have

**sv_soccar_lift_ground 0|1** -- turns horizontal curving on when the ball is on the ground

**sv_soccar_draw_magnus_force 0|1** -- draw some little red lines showing which way the magnus force is pushing the ball

**sv_soccar_magnus_debug 0|1** -- when activated, autospins the ball very fast, to ensure you see a visible effect on the ball

**sv_soccar_forcemode - 0|6** -- specify the method of adding force to the ball
    Force                                    = 0,
    Impulse                                  = 1,
    Velocity                                 = 2,
    SmoothImpulse                            = 3,
    SmoothVelocity                           = 4,
    Acceleration                             = 5,
    MAX                                      = 6


- when compiling the code; check and modify the include directories to point to the BakkesMod sdk, and the post-build step as well.

