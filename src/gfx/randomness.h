#pragma once

#include <QMap>

class Randomness
{
public:
	Randomness();
	virtual ~Randomness();

	virtual int getRandom() = 0;
	virtual Randomness* getRandomness(int n) = 0;

};

class RandomRandomness : public Randomness
{
public:
	RandomRandomness( int random, Randomness* randomness );
	virtual ~RandomRandomness();

	int getRandom();
	Randomness* getRandomness( int n );

	int m_random;
	Randomness* m_randomness;
};

class BranchRandomness : public Randomness
{
public:
	BranchRandomness();
	virtual ~BranchRandomness();

	int getRandom();
	Randomness* getRandomness( int n );

	QMap<int, Randomness*> m_randomness;
};


class NoRandomness : public Randomness
{
public:
	NoRandomness();
	virtual ~NoRandomness();

	int getRandom();
	Randomness* getRandomness( int n );

	QMap<int, Randomness*> m_randomness;
};