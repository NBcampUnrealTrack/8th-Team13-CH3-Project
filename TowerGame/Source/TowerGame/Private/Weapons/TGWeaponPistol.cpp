// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/TGWeaponPistol.h"

ATGWeaponPistol::ATGWeaponPistol()
	: ShotInterval(0.1f)
{
	Name = TEXT("Pistol");
	Power = 10;
	TriggerType = SINGLE_SHOT;
}
