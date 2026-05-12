// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Types/TGAssaultEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"

ATGAssaultEnemy::ATGAssaultEnemy()
{
	HP = 7;
	AttackDamage = 1;
	AttackInterVal = 0.4f;
	AttackRange = 200;
	EnergyDropAmount = 40;

	GetCharacterMovement()->MaxWalkSpeed = 500;
}
