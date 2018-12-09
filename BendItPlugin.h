#pragma once
#include <memory>
#pragma comment( lib, "bakkesmod.lib" )
#include "bakkesmod/plugin/bakkesmodplugin.h"

/*
Colors the prediction line can have
*/
struct LineColor
{
	unsigned char r, g, b, a; //rgba can be a value of 0-255
};

/*Predicted point in 3d space*/
struct PredictedPoint
{
	/*Location of the predicted ball*/
	Vector location;
	/*States whether it as its highest point or bounces*/
	bool isApex = false;
	Vector apexLocation = { 0,0,0 };
	Vector velocity;
	Vector angVel;
};

class BendItPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	Vector lastAppliedMagnusEffect = { 0,0,0 };
	std::shared_ptr<bool> curveOn;
	std::shared_ptr<bool> debug;
	std::shared_ptr<float> dragCoeff, liftCoeff;
	std::shared_ptr<bool> lineOn;
	std::shared_ptr<int> forceMode;
	std::shared_ptr<float> zMultiplier;
	std::shared_ptr<float> maxSpin;
	std::shared_ptr<bool> groundTurn;
	LineColor colors[2] = { { 0, 255, 0, 240 },{ 75, 0, 130, 240 } };
	int count;
public:
	BendItPlugin();
	~BendItPlugin();
	virtual void onLoad();
	virtual void onUnload();

	void OnFreeplayLoad(std::string eventName);
	void OnMaxSpinChange(std::string eventName);
	void OnFreeplayDestroy(std::string eventName);
	void OnExhibitionLoad(std::string eventName);
	void OnExhibitionDestroy(std::string eventName);
	void OnBallTick(ServerWrapper ball, void* params, std::string eventName);
	void Render(CanvasWrapper canvas);
};

