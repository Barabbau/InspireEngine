#pragma once

#include "Camera.h"

//Camera information
Camera::Camera( float Width, float Height )
{
	float positionScaler = 3.048f;

	//Camera information
	CamPosition = XMVectorSet( 0.0f, 1.83f / positionScaler, -24.0f, 0.0f );
	CamTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

	//Set the View matrix
	CamView = XMMatrixLookAtLH( CamPosition, CamTarget, camUp );

	//Set the Projection matrix
	CamProjection = XMMatrixPerspectiveFovLH( 0.4f * 3.14f, Width / Height, 0.01f, 1000.0f );
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
	CamPosition += moveBackForward * ( camForward + XMLoadFloat3( &XMFLOAT3( 0.0f,sinf(( XMVectorGetX(CamPosition) + XMVectorGetZ( CamPosition )) * 7.0f ) / 25.0f, 0.0f )) );

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	CamTarget = CamPosition + CamTarget;

	CamView = XMMatrixLookAtLH( CamPosition, CamTarget, camUp );
}