#include "MathUtilityForText.h"
#include "Vector3.h"
#include "ViewProjection.h"

class Player;

class CameraController {

public:

	Player* target_ = nullptr;

	void Initialize();
	void Update();
	void Reset();

	void setTarget(Player* target) { target_ = target; }

	const ViewProjection& GetViewProjection() const { return viewProjection_; }

private:
	ViewProjection viewProjection_;
	Vector3 targetOffset_ = {0, 0, -15.0f};

};
