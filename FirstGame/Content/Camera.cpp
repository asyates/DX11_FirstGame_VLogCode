#include "pch.h"
#include "Camera.h"


using namespace DirectX;

Camera::Camera() {
	vecCamPosition = XMVectorSet(0.0f, 1.0f, -10.0f, 0);
	vecCamLookAt = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	vecCamUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
}

void Camera::UpdateCameraLookAt(float lookAngle) {
	
	//get new x and z coordinates of unit circle cenetered on current cam position
	float new_x = XMVectorGetByIndex(vecCamPosition, 0) + cos(lookAngle);
	float new_z = XMVectorGetByIndex(vecCamPosition, 2) + sin(lookAngle);

	vecCamLookAt = XMVectorSetByIndex(vecCamLookAt, new_x, 0);
	vecCamLookAt = XMVectorSetByIndex(vecCamLookAt, new_z, 2);
}

void Camera::UpdateCameraPosition(float lookAngle, bool reverse) {
	
	//define new x and z floats
	float new_x;
	float new_z;

	//check whether we are moving forwards and backwards
	if (reverse == false) {
		//move cam position in positive direction we are looking
		new_x = XMVectorGetByIndex(vecCamPosition, 0) + movRate * cos(lookAngle);
		new_z = XMVectorGetByIndex(vecCamPosition, 2) + movRate * sin(lookAngle);
	}
	else {
		//move cam position in opposite direction to way we are looking
		new_x = XMVectorGetByIndex(vecCamPosition, 0) + movRate * cos(lookAngle-3.141592f);
		new_z = XMVectorGetByIndex(vecCamPosition, 2) + movRate * sin(lookAngle-3.141592f);
	}

	//update cam position with new x and z coords
	vecCamPosition = XMVectorSetByIndex(vecCamPosition, new_x, 0);
	vecCamPosition = XMVectorSetByIndex(vecCamPosition, new_z, 2);

	//update looking direction
	UpdateCameraLookAt(lookAngle);
	
}

XMMATRIX Camera::GetCameraView() {
	return XMMatrixLookAtLH(vecCamPosition, vecCamLookAt, vecCamUp);
}

