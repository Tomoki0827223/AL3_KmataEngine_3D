#pragma once

#include "ViewProjection.h"
#include "Player.h"

class CameraController {
public:

	void Initialize();
	void Update();


private:

	ViewProjection viewProjection;


};
