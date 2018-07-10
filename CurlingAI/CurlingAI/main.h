#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "CurlingSimulator.h"
#include "strategy.h"
#include "divide.h"
#include "Node.h"

extern HMODULE hCSDLL;

extern SIMULATION_FUNC Simulation;
extern CREATESHOT_FUNC CreateShot;
extern CREATEHITSHOT_FUNC CreateHitShot;
extern GETSCORE_FUNC GetScore;

static const int BUFSIZE = 1024;