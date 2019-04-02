#pragma once

#include "Camera.h"

//Camera information
Camera::Camera( float Width, float Height )
{
	//Camera information
	CamPosition = XMVectorSet( 0.0f, 5.0f, -4.0f, 0.0f ); //5.0f, -8.0f, 0.0f );
	CamTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	//Set the View matrix
	CamView = XMMatrixLookAtLH( CamPosition, CamTarget, camUp );

	//Set the Projection matrix
	CamProjection = XMMatrixPerspectiveFovLH( 0.4f * 3.14f, Width / Height, 0.1f, 1000.0f );
}

Camera::Camera( )
{
}

Camera::~Camera( )
{
}

void Camera::Update( float moveLeftRight, float moveBackForward )
{
	camRotationMatrix = XMMatrixRotationRollPitchYaw( CamPitch, CamYaw, 0 );
	CamTarget = XMVector3TransformCoord( DefaultForward, camRotationMatrix );
	CamTarget = XMVector3Normalize( CamTarget );

	XMMATRIX RotateYTempMatrix;
	RotateYTempMatrix = XMMatrixRotationY( CamYaw );

	//walk
	camRight = XMVector3TransformCoord( DefaultRight, RotateYTempMatrix );
	camUp = XMVector3TransformCoord( camUp, RotateYTempMatrix );
	camForward = XMVector3TransformCoord( DefaultForward, RotateYTempMatrix );

	CamPosition += moveLeftRight * camRight;
	CamPosition += moveBackForward * camForward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	CamTarget = CamPosition + CamTarget;

	CamView = XMMatrixLookAtLH( CamPosition, CamTarget, camUp );
}