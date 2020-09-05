
#include "../base/db.h"
#include "../gfx/spriteDefinition.h"

#include <QDebug>
#include <QPainter>

SpriteDefinition::SpriteDefinition( SDID sID )
{
	m_sID = sID;
}

SpriteDefinition::SpriteDefinition( const SpriteDefinition& other )
{
	uID           = other.uID;
	m_sID           = other.m_sID;
	m_type        = other.m_type;
}

SpriteDefinition::~SpriteDefinition()
{
}

BaseSpriteDefinition::BaseSpriteDefinition( SDID sID,QString tilesheet, char xOffset, char yOffset ) :
	SpriteDefinition(sID)
{
	m_xOffset = xOffset;
	m_yOffset = yOffset;
	m_tilesheet = tilesheet;
}

BaseSpriteDefinition::BaseSpriteDefinition( const BaseSpriteDefinition& other ) :
	SpriteDefinition( other )
{
	m_xOffset   = other.m_xOffset;
	m_yOffset   = other.m_yOffset;
	m_tilesheet = other.m_tilesheet;
	m_type      = "BaseSprite";
}

BaseSpriteDefinition::~BaseSpriteDefinition()
{
}

Sprite* BaseSpriteDefinition::createSprite( QStringList materialSID, Randomness* random )
{
	QPixmap pm           = m_pixmap;
	SpritePixmap* sprite = new SpritePixmap( pm, "0 0" );
	return sprite;
}

Randomness* BaseSpriteDefinition::createRandomness( QStringList materialIDs )
{
	return NULL;
}

SeasonSpriteDefinition::SeasonSpriteDefinition( SDID sID ) :
	SpriteDefinition( sID )
{
}

SeasonSpriteDefinition::SeasonSpriteDefinition( const SeasonSpriteDefinition& other ) :
	SpriteDefinition( other )
{
}

SeasonSpriteDefinition::~SeasonSpriteDefinition()
{
}

Sprite* SeasonSpriteDefinition::createSprite( QStringList materialSID, Randomness* random )
{
	SpriteSeasons* ss = new SpriteSeasons;
	for ( auto key : m_seasons.keys() )
	{
		ss->m_sprites.insert( key, m_seasons.value( key )->createSprite( materialSID, random ) );
	}
	return ss;
}

RotationSpriteDefinition::RotationSpriteDefinition( SDID sID ) :
	SpriteDefinition( sID )
{
}

RotationSpriteDefinition::RotationSpriteDefinition( const RotationSpriteDefinition& other ) :
	SpriteDefinition( other )
{
}

RotationSpriteDefinition::~RotationSpriteDefinition()
{
}

Sprite* RotationSpriteDefinition::createSprite( QStringList materialSID, Randomness* random )
{
	SpriteRotations* sr = new SpriteRotations;

	sr->m_sprites.push_back( m_rotations.value( "FR" )->createSprite( materialSID, random ) );
	sr->m_sprites.push_back( m_rotations.value( "FL" )->createSprite( materialSID, random ) );
	sr->m_sprites.push_back( m_rotations.value( "BL" )->createSprite( materialSID, random ) );
	sr->m_sprites.push_back( m_rotations.value( "BR" )->createSprite( materialSID, random ) );

	return sr;
}

FramesSpriteDefinition::FramesSpriteDefinition( SDID sID ) :
	SpriteDefinition( sID )
{
}

FramesSpriteDefinition::FramesSpriteDefinition( const FramesSpriteDefinition& other ) :
	SpriteDefinition( other )
{
}

FramesSpriteDefinition::~FramesSpriteDefinition()
{
}

Sprite* FramesSpriteDefinition::createSprite( QStringList materialSID, Randomness* random )
{
	SpriteFrames* sf = new SpriteFrames;
	for ( auto child : m_frames )
	{
		sf->m_sprites.push_back( child->createSprite( materialSID, random ) );
	}
	return sf;
}

RandomSpriteDefinition::RandomSpriteDefinition( SDID sID ) :
	SpriteDefinition( sID )
{
}

RandomSpriteDefinition::RandomSpriteDefinition( const RandomSpriteDefinition& other ) :
	SpriteDefinition( other )
{
}

RandomSpriteDefinition::~RandomSpriteDefinition()
{
}

Sprite* RandomSpriteDefinition::createSprite( QStringList materialSID, Randomness* random )
{
	int randomNumber = random->m_random;
	Randomness* randomS = random->m_randomness.value( randomNumber );
	return m_sprites.at( randomNumber )->createSprite( materialSID, randomS );
}

Randomness* RandomSpriteDefinition::createRandomness( QStringList materialIDs )
{

	int sum   = m_sum;
	int ran   = rand() % sum;
	int total = 0;
	for ( int i = 0; i < m_weights.size(); ++i )
	{
		total += m_weights[i];
		if ( ran < total )
		{
			Randomness random = Randomness( i );
			Randomness* subRand = m_sprites.at( i )->createRandomness( materialIDs );
			random.m_randomness = subRand->;
			break;
		}
	}

}

CombineSpriteDefinition::CombineSpriteDefinition( SDID sID ) :
	SpriteDefinition( sID )
{
}

CombineSpriteDefinition::CombineSpriteDefinition( const CombineSpriteDefinition& other ) :
	SpriteDefinition( other )
{
}

CombineSpriteDefinition::~CombineSpriteDefinition()
{
}

Sprite* CombineSpriteDefinition::createSprite( QStringList materialSID, Randomness* random )
{
	Sprite* s = m_sprites.at( 0 )->createSprite( materialSID, random );

	for ( int i = 1; i < m_sprites.size(); ++i )
	{
		Sprite* s2 = m_sprites.at( i )->createSprite( materialSID, random );
		s->combine( s2 ,"",0,0); //TODO new combine
	}
	return s;
}

TypeSpriteDefinition::TypeSpriteDefinition( SDID sID ) :
	SpriteDefinition( sID )
{
}

TypeSpriteDefinition::TypeSpriteDefinition( const TypeSpriteDefinition& other ) :
	SpriteDefinition( other )
{
}

TypeSpriteDefinition::~TypeSpriteDefinition()
{
}

MaterialSpriteDefinition::MaterialSpriteDefinition( SDID sID ) :
	SpriteDefinition( sID )
{
	m_type = "ByMaterials";
}

MaterialSpriteDefinition::MaterialSpriteDefinition( const MaterialSpriteDefinition& other ) :
	SpriteDefinition( other )
{
}

MaterialSpriteDefinition::~MaterialSpriteDefinition()
{
}

Sprite* MaterialSpriteDefinition::createSprite( QStringList materialSID, Randomness* random )
{

	return nullptr;
}

EffectSpriteDefinition::EffectSpriteDefinition( SDID sID, SDID spriteDef ) :
	SpriteDefinition( sID )
{
	m_spriteDef = spriteDef;
}

EffectSpriteDefinition::EffectSpriteDefinition( const EffectSpriteDefinition& other ) :
	SpriteDefinition( other )
{
}

EffectSpriteDefinition::~EffectSpriteDefinition()
{
}

TintSpriteDefinition::TintSpriteDefinition( SDID sID, SDID spriteDef, QString tint ) :
	EffectSpriteDefinition( sID, spriteDef )
{
	m_tint = tint;
}

TintSpriteDefinition::TintSpriteDefinition( SDID sID, SDID spriteDef, int material ) :
	EffectSpriteDefinition( sID, spriteDef )
{
	//TODO
}

TintSpriteDefinition::TintSpriteDefinition( const TintSpriteDefinition& other ) :
	EffectSpriteDefinition( other )
{
}

TintSpriteDefinition::~TintSpriteDefinition()
{
}

FlipSpriteDefinition::FlipSpriteDefinition( SDID sID, SDID spriteDef, QString effect ) :
	EffectSpriteDefinition( sID, spriteDef )
{
	m_effect = effect;
}

FlipSpriteDefinition::FlipSpriteDefinition( const FlipSpriteDefinition& other ) :
	EffectSpriteDefinition( other )
{
}

FlipSpriteDefinition::~FlipSpriteDefinition()
{
}

RotateSpriteDefinition::RotateSpriteDefinition( SDID sID, SDID spriteDef ) :
	EffectSpriteDefinition( sID, spriteDef )
{
}

RotateSpriteDefinition::RotateSpriteDefinition( const RotateSpriteDefinition& other ) :
	EffectSpriteDefinition( other )
{
}

RotateSpriteDefinition::~RotateSpriteDefinition()
{
}


MapSpriteDefinition::MapSpriteDefinition( SDID sID ) :
	SpriteDefinition( sID )
{
}

MapSpriteDefinition::MapSpriteDefinition( const MapSpriteDefinition& other ) :
	SpriteDefinition( other )
{
	m_sprites = other.m_sprites;
}

MapSpriteDefinition::~MapSpriteDefinition()
{
}

Randomness* MapSpriteDefinition::createRandomness( QStringList materialIDs )
{
	Randomness r =  Randomness();
	for ( int i =0 ; i < m_sprites.size(); i++ )
	{
		Randomness* rSub = m_sprites.values().at(i)->createRandomness( materialIDs );
		if (rSub)
		{
			r.m_randomness.insert( i, rSub );
		}
	}
	return &r;
}

