// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Types/TGNormalEnemy.h"

#include "GameFramework/CharacterMovementComponent.h"

ATGNormalEnemy::ATGNormalEnemy()
{
	HP = 10;
	StructureAttackDamage = 1;
	StructureAttackInterval = 0.5f;
	StructureAttackRange = 200;
	EnergyDropAmount = 30;

	GetCharacterMovement()->MaxWalkSpeed = 300;
}
