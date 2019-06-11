#pragma once
#include "pch.h"

using namespace DirectX;

class Camera {

public:
	Camera();
	//void MoveCamera(std::array<bool, 4> wasd);
	XMMATRIX GetCameraView();
	XMVECTOR GetCameraPosition();
	void UpdateCameraLookAtXZ(float lookAngle);
	void TiltCameraY(bool up);
	void UpdateCameraPosition(float lookAngle, bool reverse);
	void AdjustCameraPositionY(float adjustment);

private:
	XMVECTOR vecCamPosition;
	XMVECTOR vecCamLookAt;
	XMVECTOR vecCamUp;

	float movRate = 0.1f; //camera position movement rate
	float yaxisRate = 0.02f; //rate camera look at vector changes in y direction
};
