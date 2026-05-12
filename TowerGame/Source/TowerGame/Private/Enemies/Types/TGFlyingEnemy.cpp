// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Types/TGFlyingEnemy.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#define ECC_FlyingEnemy ECC_GameTraceChannel2

ATGFlyingEnemy::ATGFlyingEnemy()
{
	HP = 10;
	AttackDamage = 1;
	AttackInterVal = 0.5f;
	AttackRange = 500;
	EnergyDropAmount = 40;

	NavigationLocationOffset = FVector(0,0,250);

	GetCharacterMovement()->MaxWalkSpeed = 200;

	UCapsuleComponent* CapsuleCollision = GetCapsuleComponent();
	if (!CapsuleCollision) return;

	// Collision FlyingEnemy로 설정 + FlyingEnemy 충돌 무시
	CapsuleCollision->SetCollisionObjectType(ECC_FlyingEnemy);
	CapsuleCollision->SetCollisionResponseToChannel(ECC_FlyingEnemy, ECR_Ignore);
}
