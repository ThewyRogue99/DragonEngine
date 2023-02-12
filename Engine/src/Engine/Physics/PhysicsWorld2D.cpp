#include "depch.h"
#include "PhysicsWorld2D.h"

#include "Engine/Debug/Debug.h"

#include "Engine/Core/TaskQueue.h"
#include "Engine/Scene/Scene.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

namespace Engine
{
	static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
		case Rigidbody2DComponent::BodyType::Static:
			return b2_staticBody;
		case Rigidbody2DComponent::BodyType::Dynamic:
			return b2_dynamicBody;
		case Rigidbody2DComponent::BodyType::Kinematic:
			return b2_kinematicBody;
		}

		DE_ASSERT(false, "Unknown body type");
		return b2_staticBody;
	}

	PhysicsWorld2D::~PhysicsWorld2D()
	{
		End();
	}

	void PhysicsWorld2D::Begin(Scene* SceneRef, const glm::vec2& Gravity)
	{
		if (bShouldRun)
			End();
		else
			bShouldRun = true;

		CurrentScene = SceneRef;
		WorldGravity = Gravity;

		b2PhysicsWorld = new b2World({ WorldGravity.x, WorldGravity.y });

		auto view = CurrentScene->SceneRegistry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = Entity(e, CurrentScene);
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef;
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.Set(transform.Position.x, transform.Position.y);
			bodyDef.angle = glm::radians(transform.Rotation.z);
			bodyDef.fixedRotation = rb2d.FixedRotation;

			b2Body* body = b2PhysicsWorld->CreateBody(&bodyDef);
			rb2d.RuntimeBody = body;

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transform.Scale.x, bc2d.Size.y * transform.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

			if (entity.HasComponent<CircleCollider2DComponent>())
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.Set(cc2d.Offset.x, cc2d.Offset.y);
				circleShape.m_radius = (cc2d.Radius * transform.Scale.x) / 2.f;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}

		UpdateThread = new std::thread(BIND_CLASS_FN(UpdateTask));
	}

	void PhysicsWorld2D::End()
	{
		if (bShouldRun)
		{
			bShouldRun = false;

			delete b2PhysicsWorld;
			b2PhysicsWorld = nullptr;

			if (UpdateThread->joinable())
			{
				UpdateCondition.notify_all();
				UpdateThread->join();
			}

			delete UpdateThread;
			UpdateThread = nullptr;
		}
	}

	void PhysicsWorld2D::UpdateTask()
	{
		while (true)
		{
			const int32_t velocityIterations = 6;
			const int32_t positionIterations = 2;

			std::unique_lock<std::mutex> lock(UpdateMutex);
			UpdateCondition.wait(lock);

			if (!bShouldRun) return;

			b2PhysicsWorld->Step(deltaTime ? deltaTime : (1.f / 60.f), velocityIterations, positionIterations);

			// Retrieve transform from Box2D
			auto view = CurrentScene->SceneRegistry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = Entity(e, CurrentScene);
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				if (body)
				{
					const auto& position = body->GetPosition();
					transform.Position.x = position.x;
					transform.Position.y = position.y;
					transform.Rotation.z = glm::degrees(body->GetAngle());
				}
			}
		}
	}

	void PhysicsWorld2D::Update(float DeltaTime)
	{
		deltaTime = DeltaTime;
		UpdateCondition.notify_all();
	}
}