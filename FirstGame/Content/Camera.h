#pragma once
#include "pch.h"

using namespace DirectX;

class Camera {

public:
	Camera();
	//void MoveCamera(std::array<bool, 4> wasd);
	XMMATRIX GetCameraView();
	XMVECTOR GetCameraPosition();
	
	void UpdateCameraLookAtXZ(float lookAngleXZ);
	void TiltCameraY(bool up);
	void AdjustCameraPosition(float m, float lookAngleXZ);
	void AdjustCameraPositionY(float adjustment);

private:
	XMVECTOR vecCamPosition;
	XMVECTOR vecCamLookAt;
	XMVECTOR vecCamUp;

	float yaxisRate = 0.02f; //rate camera look at vector changes in y direction
};
