#pragma once

//Include and link appropriate libraries and headers//
#include <windows.h>
#include <xnamath.h>

class Camera
{
	public:
	XMVECTOR CamPosition;
	XMVECTOR CamTarget;
	XMVECTOR camUp;

	XMVECTOR camForward = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	XMVECTOR camRight = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );

	float CamYaw = 0.0f;
	float CamPitch = 0.0f;
	float FarClip = 1000.0f;
	float NearClip = 0.01f;

	XMMATRIX camRotationMatrix;

	XMMATRIX CamView;
	XMMATRIX CamProjection;


	
	Camera( );
	Camera( float Width, float Height );
	~Camera( );
	void Update( float moveLeftRight = 0.0f, float moveBackForward = 0.0f );


	private:
	XMVECTOR DefaultForward = XMVectorSet( 0.0f, 0.0f, 1.0f, 0.0f );
	XMVECTOR DefaultRight = XMVectorSet( 1.0f, 0.0f, 0.0f, 0.0f );
};