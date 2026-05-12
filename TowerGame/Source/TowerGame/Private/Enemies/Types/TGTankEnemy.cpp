// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Types/TGTankEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"

ATGTankEnemy::ATGTankEnemy()
{
	HP = 15;
	AttackDamage = 2;
	AttackInterVal = 0.9f;
	AttackRange = 200;
	EnergyDropAmount = 50;

	GetCharacterMovement()->MaxWalkSpeed = 200;
}
