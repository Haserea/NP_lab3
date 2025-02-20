#pragma once

struct QuadraticEquation
{
	double a;
	double b;
	double c;
};

enum SquareRootResult {NO_ROOT, ONE_ROOT, TWO_ROOT};

struct SquareRootData
{
	enum quareRootResult result;

	double x1;
	
	double x2;
};