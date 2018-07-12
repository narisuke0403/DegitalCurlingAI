#pragma once
#include "Node.h"
#include "divide.h"
#include "main.h"
#include <vector>
#include <stdio.h>

class Qlearning {
public:
	std::array<Node, 16> *QNode;
	Qlearning();
};
