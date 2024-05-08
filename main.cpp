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

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	Vector3 v1{ 1.2f,-3.9f,2.5f };

	Vector3 v2{ 2.8f,0.4f,-1.3f };

	Vector3 cross = Cross(v1, v2);

	Vector3 cameraPosition{ 0.0f,0.0f,-10.0f };

	Vector3 rotate{};

	Vector3 translate{};

	Vector3 kLocalVertices[3]{ {0.0f,-50.0f,0.0f},{50.0f,50.0f,0.0f},{-50.0f,50.0f,0.0f} };

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

		rotate.y += 0.04f;

		if (keys[DIK_A])
		{
			translate.x += 1.0f;
		}

		if (keys[DIK_D])
		{
			translate.x -= 1.0f;
		}

		if (keys[DIK_W])
		{
			translate.z -= 1.0f;
		}

		if (keys[DIK_S])
		{
			translate.z += 1.0f;
		}

		Matrix4x4 worldMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);

		Matrix4x4 cameraMatrix = MatrixFunction::MakeAffineMatrix({ 1.0f, 1.0f,1.0f }, { 0.0f,0.0f,0.0f }, cameraPosition);

		Matrix4x4 viewMatrix = MatrixFunction::Inverse(cameraMatrix);

		Matrix4x4 projectionMatrix = MatrixFunction::MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.f);

		Matrix4x4 worldViewProjectionMatrix = MatrixFunction::Multiply(viewMatrix, projectionMatrix);

		worldViewProjectionMatrix = MatrixFunction::Multiply(worldMatrix, worldViewProjectionMatrix);

		Matrix4x4 viewportMatrix = MatrixFunction::MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 1.0f);

		Vector3 screenVertices[3];

		for (uint32_t i = 0; i < 3; ++i)
		{
			Vector3 ndcVertex = MakeTransform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = MakeTransform(ndcVertex, viewportMatrix);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawTriangle(int(screenVertices[0].x), int(screenVertices[0].y), int(screenVertices[1].x), int(screenVertices[1].y), int(screenVertices[2].x), int(screenVertices[2].y), RED, kFillModeSolid);

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
