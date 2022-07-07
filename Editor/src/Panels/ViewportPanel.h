#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Scene/Entity.h"
#include "EditorPanel.h"
#include "../Scene/EditorScene.h"
#include "Engine/Renderer/Framebuffer.h"

#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"

#include <ImGuizmo/ImGuizmo.h>

namespace Engine
{
	class ViewportPanel : public EditorPanel
	{
	public:
		ViewportPanel();

		virtual void OnCreate() override;

		virtual void OnUpdate(float DeltaTime) override;

		virtual void OnRender(float DeltaTime) override;

		virtual void OnResize(uint32_t width, uint32_t height) override;

		virtual void OnEvent(Event& event) override;

	protected:
		virtual void OnData(const CString& Name, void* Data, size_t size);

	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& event);
		bool OnKeyPressedEvent(KeyPressedEvent& event);

	private:
		EditorScene* ActiveScene = nullptr;
		Ref<Framebuffer> m_FrameBuffer = nullptr;

		Entity HoveredEntity = { };

		int GizmoType = ImGuizmo::OPERATION::TRANSLATE;

		bool bIsViewportFocused = false, bIsViewportHovered = false;
	};
}
