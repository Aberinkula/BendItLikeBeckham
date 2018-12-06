#define LINMATH_H //Conflicts with linmath.h if we done declare this here

#include "BendItPlugin.h"
#include "bakkesmod/wrappers/GameEvent/ServerWrapper.h"
#include "bakkesmod/wrappers/GameObject/BallWrapper.h"
#include "bakkesmod/wrappers/GameObject/CarWrapper.h"
#include "bakkesmod\wrappers\GameEvent\TutorialWrapper.h"
#include "bakkesmod/wrappers/arraywrapper.h"
#include <sstream>

BAKKESMOD_PLUGIN(BendItPlugin, "Bend It Like Beckham  Questions/Bugs?  Go to BakkesMod discord or DM http://twitter.com/FrancesElMute", "1.05", PLUGINTYPE_FREEPLAY | PLUGINTYPE_CUSTOM_TRAINING)

BendItPlugin::BendItPlugin()
{
	count = 0;
}

BendItPlugin::~BendItPlugin()
{
}

void BendItPlugin::onLoad()
{
	curveOn = make_shared<bool>(true);
	cvarManager->registerCvar("sv_soccar_curve", "0", "Curve the ball when it spins", true, true, 0, true, 1).bindTo(curveOn);
	cvarManager->getCvar("sv_soccar_curve").addOnValueChanged(std::bind(&BendItPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	
	dragCoeff = make_shared<float>(0.5);
	cvarManager->registerCvar("sv_soccar_drag_coefficient", "0.005", "Drag coefficient for ball", true, true, 0, true, 1).bindTo(dragCoeff);

	liftCoeff = make_shared<float>(1.0);
	cvarManager->registerCvar("sv_soccar_lift_coefficient", "1.0", "Lift coefficient for ball", true, true, 0, true, 100).bindTo(liftCoeff);

	lineOn = make_shared<bool>(true);
	cvarManager->registerCvar("sv_soccar_draw_magnus_force", "0", "draw the magnus force applied to the ball", true, true, 0, true, 1).bindTo(lineOn);

	debug = make_shared<bool>(false);
	cvarManager->registerCvar("sv_soccar_magnus_debug", "0", "autospin the ball to test magnus effect", true, true, 0, true, 1).bindTo(debug);

	forceMode = make_shared<int>(0);
	cvarManager->registerCvar("sv_soccar_forcemode", "0", "Force mode to apply", true, true, 0, true, 6).bindTo(forceMode);

	
	gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.OnInit", bind(&BendItPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Tutorial_TA.Destroyed", bind(&BendItPlugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartPlayTest", bind(&BendItPlugin::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.Destroyed", bind(&BendItPlugin::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnInit", bind(&BendItPlugin::OnExhibitionLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", bind(&BendItPlugin::OnExhibitionDestroy, this, std::placeholders::_1));


	OnFreeplayLoad("init");
}
void BendItPlugin::OnExhibitionLoad(std::string eventName)
{
	cvarManager->log(std::string("OnExhibitionLoad") + eventName);
	if (*curveOn) {

		gameWrapper->RegisterDrawable(std::bind(&BendItPlugin::Render, this, std::placeholders::_1));
		gameWrapper->HookEventWithCaller<ServerWrapper>("Function GameEvent_Soccar_TA.Active.Tick", std::bind(&BendItPlugin::OnBallTick, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else {
		gameWrapper->UnregisterDrawables();
		gameWrapper->UnhookEvent("Function GameEvent_Soccar_TA.Active.Tick");
	}
}

void BendItPlugin::OnExhibitionDestroy(std::string eventName)
{
		gameWrapper->UnregisterDrawables();
		gameWrapper->UnhookEvent("Function GameEvent_Soccar_TA.Active.Tick");
}
void BendItPlugin::OnFreeplayLoad(std::string eventName)
{

	cvarManager->log(std::string("OnFreeplayLoad") + eventName);
	if (*curveOn) {

		gameWrapper->RegisterDrawable(std::bind(&BendItPlugin::Render, this, std::placeholders::_1));
		gameWrapper->HookEventWithCaller<ServerWrapper>("Function GameEvent_Tutorial_FreePlay_TA.Active.Tick", std::bind(&BendItPlugin::OnBallTick, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	}
	else {
		gameWrapper->UnregisterDrawables();
		gameWrapper->UnhookEvent("Function GameEvent_Tutorial_FreePlay_TA.Active.Tick");
	}
}

void BendItPlugin::OnFreeplayDestroy(std::string eventName)
{
	gameWrapper->UnregisterDrawables();
	gameWrapper->UnhookEvent("Function GameEvent_Tutorial_FreePlay_TA.Active.Tick");
}

void BendItPlugin::OnBallTick(ServerWrapper server, void * params, std::string eventName)
{
	if (!gameWrapper->IsInGame()) { //Should never happen, but rather safe than sorry
		return;
	}
	BallWrapper ballWrapper = server.GetBall();
	if (ballWrapper.IsNull())
		return;
	Rotator rotator = ballWrapper.GetRotation();
	Vector a = ballWrapper.GetAngularVelocity();
	Vector v = ballWrapper.GetVelocity();

	if (*debug) {
		a.Z = 500.0f;
		ballWrapper.SetAngularVelocity(a, false);	 
	}

	Vector magnusEffect = Vector::cross(v, a) * -1.0f;

	magnusEffect.Z *= (*liftCoeff);
	magnusEffect.X *= (*liftCoeff);
	magnusEffect.Y *= (*liftCoeff);

	lastAppliedMagnusEffect = magnusEffect;

	float drag = -(*dragCoeff * v.magnitude());
	Vector dragEffect = Vector(v.X * drag, v.Y * drag, v.Z*drag);
	if (v.Z != 0) {  // ball is off ground, apply magnus effect
		ballWrapper.AddForce(dragEffect, (char)(*forceMode));
		ballWrapper.AddForce(magnusEffect, (char)(*forceMode));
	}
}

#include <iostream>     // std::cout
#include <fstream> 

void BendItPlugin::Render(CanvasWrapper canvas)
{

	if (*curveOn /*&& gameWrapper->IsInGame()*/)
	{
		ServerWrapper server = gameWrapper->GetGameEventAsServer();
		if (server.IsNull()) {
			//cvarManager->log("null");
			return;
		}

		BallWrapper ballWrapper = server.GetBall();
		if (ballWrapper.IsNull())
			return;   // @Bakkes - just added this

		Vector v = ballWrapper.GetVelocity();

		if (*lineOn && lastAppliedMagnusEffect.magnitude() > 0.0f && v.Z != 0.0f) {
			// draw the magnusEffect vector
			Vector magnusDrawVector = lastAppliedMagnusEffect * 3.0f;
			if (magnusDrawVector.magnitude() > 150.0f) {
				magnusDrawVector = magnusDrawVector / magnusDrawVector.magnitude() * 150.0f;
			}

			ArrayWrapper<CarWrapper> cars = server.GetCars();

			canvas.SetColor(255, 0, 0, 200);
			canvas.DrawLine(canvas.Project(ballWrapper.GetLocation()), canvas.Project(ballWrapper.GetLocation() + magnusDrawVector));
			canvas.DrawLine(canvas.Project(ballWrapper.GetLocation() + Vector(0, 0, 30)), canvas.Project(ballWrapper.GetLocation() + magnusDrawVector));
			canvas.DrawLine(canvas.Project(ballWrapper.GetLocation() - Vector(0, 0, 30)), canvas.Project(ballWrapper.GetLocation() + magnusDrawVector));
			canvas.DrawLine(canvas.Project(ballWrapper.GetLocation() + Vector(30, 0, 0)), canvas.Project(ballWrapper.GetLocation() + magnusDrawVector));
			canvas.DrawLine(canvas.Project(ballWrapper.GetLocation() + Vector(30, 0, 0)), canvas.Project(ballWrapper.GetLocation() + magnusDrawVector));

			//canvas.SetColor(255, 255, 0, 200);
			//Vector angularVelocityDrawVector = a / a.magnitude() * 350.0f;
			//canvas.DrawLine(canvas.Project(ballWrapper.GetLocation()), canvas.Project(ballWrapper.GetLocation() + angularVelocityDrawVector));

		}
	}
}

void BendItPlugin::onUnload()
{
}

