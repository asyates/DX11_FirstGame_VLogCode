#include "pch.h"
#include "Camera.h"


using namespace DirectX;

Camera::Camera() {
	vecCamPosition = XMVectorSet(0.0f, 1.0f, -10.0f, 1.0f);
	vecCamLookAt = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	vecCamUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
}

//adjust camera look at vector in XZ plane
void Camera::UpdateCameraLookAtXZ(float lookAngleXZ) {
	
	//get new x and z coordinates of unit circle cenetered on current cam position
	float new_x = XMVectorGetByIndex(vecCamPosition, 0) + cos(lookAngleXZ);
	float new_z = XMVectorGetByIndex(vecCamPosition, 2) + sin(lookAngleXZ);

	vecCamLookAt = XMVectorSetByIndex(vecCamLookAt, new_x, 0);
	vecCamLookAt = XMVectorSetByIndex(vecCamLookAt, new_z, 2);
}

//Move Camera position by float m in the direction lookAngleXZ
void Camera::MoveCameraForwardBack(float m, float lookAngleXZ) {

	//define new x and z floats
	float new_x;
	float new_z;

	//check whether we are moving forwards and backwards
	if (m > 0) {
		//move cam position in positive direction we are looking
		new_x = XMVectorGetByIndex(vecCamPosition, 0) + abs(m) * cos(lookAngleXZ);
		new_z = XMVectorGetByIndex(vecCamPosition, 2) + abs(m) * sin(lookAngleXZ);
	}
	else {
		//move cam position in opposite direction to way we are looking
		new_x = XMVectorGetByIndex(vecCamPosition, 0) + abs(m) * cos(lookAngleXZ - pi);
		new_z = XMVectorGetByIndex(vecCamPosition, 2) + abs(m) * sin(lookAngleXZ - pi);
	}

	//update cam position with new x and z coords
	vecCamPosition = XMVectorSetByIndex(vecCamPosition, new_x, 0);
	vecCamPosition = XMVectorSetByIndex(vecCamPosition, new_z, 2);
}

//Move Camera position by float m in sideways direction to lookAngleXZ
void Camera::MoveCameraSideToSide(float m, float lookAngleXZ) {

	//define new x and z floats
	float new_x;
	float new_z;

	//check whether we are moving forwards and backwards
	if (m > 0) {
		//move cam position right of where we are looking
		new_x = XMVectorGetByIndex(vecCamPosition, 0) + abs(m) * cos(lookAngleXZ - pi/2);
		new_z = XMVectorGetByIndex(vecCamPosition, 2) + abs(m) * sin(lookAngleXZ - pi/2);
	}
	else {
		//move cam position left of where we we are looking
		new_x = XMVectorGetByIndex(vecCamPosition, 0) + abs(m) * cos(lookAngleXZ + pi/2);
		new_z = XMVectorGetByIndex(vecCamPosition, 2) + abs(m) * sin(lookAngleXZ + pi/2);
	}

	//update cam position with new x and z coords
	vecCamPosition = XMVectorSetByIndex(vecCamPosition, new_x, 0);
	vecCamPosition = XMVectorSetByIndex(vecCamPosition, new_z, 2);
}

// Move Y coordinate of camera position by the amount 'adjustment' (float)
void Camera::AdjustCameraPositionY(float adjustment) {
	
	float new_yPos = XMVectorGetByIndex(vecCamPosition, 1) + adjustment;
	vecCamPosition = XMVectorSetByIndex(vecCamPosition, new_yPos, 1);
	
	float new_yLook = XMVectorGetByIndex(vecCamLookAt, 1) + adjustment;
	vecCamLookAt = XMVectorSetByIndex(vecCamLookAt, new_yLook, 1);
}

void Camera::UpdateCameraLookAtY(float lookAngleXZ, float lookAngleY) {
	
	float new_x;
	float new_y;
	float new_z;

	//move cam look direction based on lookAngleY
	new_x = XMVectorGetByIndex(vecCamPosition, 0) + sin(pi / 2 - lookAngleY)*cos(lookAngleXZ);
	new_y = XMVectorGetByIndex(vecCamPosition, 1) + cos(pi/2- lookAngleY);
	new_z = XMVectorGetByIndex(vecCamPosition, 2) + sin(pi / 2 - lookAngleY)*sin(lookAngleXZ);

	vecCamLookAt = XMVectorSet(new_x, new_y, new_z, 0.0f);
}

XMMATRIX Camera::GetCameraView() {
	return XMMatrixLookAtLH(vecCamPosition, vecCamLookAt, vecCamUp);
}

XMVECTOR Camera::GetCameraPosition() {
	return vecCamPosition;
}

