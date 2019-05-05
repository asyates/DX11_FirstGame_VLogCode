#pragma once
#include "pch.h"

using namespace DirectX;

class Camera {

public:
	Camera();
	void MoveCamera(std::array<bool, 4> wasd);
	XMMATRIX GetCameraView();

private:
	XMVECTOR vecCamPosition;
	XMVECTOR vecCamLookAt;
	XMVECTOR vecCamUp;

	float mov_rate = 0.05f; //camera movement rate

};
