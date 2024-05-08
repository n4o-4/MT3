#include <Novice.h>
#include "MatrixFunction.h"
#include <cstdint>
#include <cassert>


const char kWindowTitle[] = "LC1C_25_ミヤモト_ナオヤ_ゲームタイトル";

Vector3 Cross(const Vector3& v1, const Vector3& v2) {

	Vector3 result;

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
}

Vector3 MakeTransform(const Vector3& vector, const Matrix4x4& matrix)
{
	Vector3 resultVector;
	resultVector.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	resultVector.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	resultVector.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	//assert(w != 0.0f);
	resultVector.x /= w;
	resultVector.y /= w;
	resultVector.z /= w;

	return resultVector;
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix)
{
	const float kGridHalfWidth = 2.0f;                                       // Gridの半分の幅
	const uint32_t kSubdivision = 10;                                        // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);  // 1つ分の長さ
	
	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {

		Vector3 startPosition{ 0.0f,0.0f,-2.0f };

		Vector3 endPosition{ 0.0f,0.0f,2.0f };
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

	

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
