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

	void MoveCameraForwardBack(float m, float lookAngleXZ);
	void MoveCameraSideToSide(float m, float lookAngleXZ);
	void AdjustCameraPositionY(float adjustment);
	void UpdateCameraLookAtY(float lookAngleXZ, float lookAngleY);

private:
	XMVECTOR vecCamPosition;
	XMVECTOR vecCamLookAt;
	XMVECTOR vecCamUp;

	float pi = 3.141592f;
	float yaxisRate = 0.02f; //rate camera look at vector changes in y direction
};
