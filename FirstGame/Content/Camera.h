#pragma once
#include "pch.h"

using namespace DirectX;

class Camera {

public:
	Camera();
	//void MoveCamera(std::array<bool, 4> wasd);
	XMMATRIX GetCameraView();
	void UpdateCameraLookAt(float lookAngle);
	void UpdateCameraPosition(float lookAngle, bool reverse);

private:
	XMVECTOR vecCamPosition;
	XMVECTOR vecCamLookAt;
	XMVECTOR vecCamUp;

	float movRate = 0.05f; //camera position movement rate

};
