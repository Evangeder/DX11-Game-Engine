#pragma once
#include "VoxaNovusConstantBuffers.h"
#include "VoxaNovusDrawable.h"
#include <DirectXMath.h>

class TransformCbuf : public Bindable {
private:
	struct Transforms {
		DirectX::XMMATRIX modelViewProj;
		DirectX::XMMATRIX model;
	};
public:
	TransformCbuf(Graphics& gfx, const Drawable& parent);
	void Bind(Graphics& gfx) noexcept override;
private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	const Drawable& parent;
};