#include "Player.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include "MathUtilityForText.h"

class Player;

class CameraController {

public:
	void Initialize();
	void Update();
	void Reset();

	void setTarget(Player* target) { target_ = target; }

	const ViewProjection& GetViewProjection() const { return viewProjection_; }

private:
	ViewProjection viewProjection_;
	Player* target_ = nullptr;
	Vector3 targetOffset_ = {0, 0, -15.0f}; // カメラのオフセット
};
