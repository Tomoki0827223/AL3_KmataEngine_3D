#include "Player.h"
#include "Vector3.h"
#include "ViewProjection.h"

class Player;

class CameraController {

public:


	void Initialize(ViewProjection* viewProjection, Player* target);
	void Update();
	void Reset();

	void setTarget(Player* target) { target_ = target; }

private:

	ViewProjection* viewProjection_ = nullptr;
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0, 0, -15.0f}; // カメラのオフセット

};
