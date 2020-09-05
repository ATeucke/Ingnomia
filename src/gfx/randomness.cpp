
#include "../gfx/randomness.h"



Randomness::Randomness()
{
}

RandomRandomness::RandomRandomness( int random, Randomness* randomness ) :
	Randomness()
{
	m_random = random;
	m_randomness = randomness;
}

RandomRandomness::~RandomRandomness()
{
	m_randomness->~Randomness();
}

int RandomRandomness::getRandom()
{
	return m_random;
}

Randomness* RandomRandomness::getRandomness( int n )
{
	return m_randomness;
}

BranchRandomness::BranchRandomness() :
	Randomness()
{
}

BranchRandomness::~BranchRandomness()
{
	for ( auto val : m_randomness.values() )
	{
		( *val ).~Randomness();
	}
}

int BranchRandomness::getRandom()
{
	return -1;
}

Randomness* BranchRandomness::getRandomness( int n )
{
	return m_randomness.value( n );
}
