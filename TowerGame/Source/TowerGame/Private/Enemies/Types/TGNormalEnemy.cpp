// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Types/TGNormalEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"

ATGNormalEnemy::ATGNormalEnemy()
{
	HP = 10;
	AttackDamage = 1;
	AttackInterVal = 0.5f;
	AttackRange = 200;
	EnergyDropAmount = 30;

	GetCharacterMovement()->MaxWalkSpeed = 300;
}
