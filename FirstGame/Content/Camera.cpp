#include "pch.h"
#include "Camera.h"


using namespace DirectX;

Camera::Camera() {
	vecCamPosition = XMVectorSet(2.0f, 3.0f, -10.0f, 0);
	vecCamLookAt = XMVectorSet(0, 0, 0, 0);
	vecCamUp = XMVectorSet(0, 1, 0, 0);

}
//Move Camera, recalled that array shows [Up, Left, Down, Right] movements.
void Camera::MoveCamera(std::array<bool, 4> wasd) {
	if (wasd[0] == true) {
		float new_y = XMVectorGetByIndex(vecCamLookAt, 1) + mov_rate;
		vecCamLookAt = XMVectorSetByIndex(vecCamLookAt, new_y, 1);
	};

	if (wasd[1] == true) {
		float new_y = XMVectorGetByIndex(vecCamLookAt, 0) - mov_rate;
		vecCamLookAt = XMVectorSetByIndex(vecCamLookAt, new_y, 0);
	};

	if (wasd[2] == true) {
		float new_y = XMVectorGetByIndex(vecCamLookAt, 1) - mov_rate;
		vecCamLookAt = XMVectorSetByIndex(vecCamLookAt, new_y, 1);
	};

	if (wasd[3] == true) {
		float new_y = XMVectorGetByIndex(vecCamLookAt, 0) + mov_rate;
		vecCamLookAt = XMVectorSetByIndex(vecCamLookAt, new_y, 0);
	};
}

XMMATRIX Camera::GetCameraView() {
	return XMMatrixLookAtLH(vecCamPosition, vecCamLookAt, vecCamUp);
}