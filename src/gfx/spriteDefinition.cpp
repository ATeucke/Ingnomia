
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

/******************************** BaseSpriteDefinition  ********************************************/

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
	return &NoRandomness();
}


/******************************** BranchingSpriteDefinition  ********************************************/

BranchingSpriteDefinition::BranchingSpriteDefinition( SDID sID ) :
	SpriteDefinition( sID )
{
}

BranchingSpriteDefinition::BranchingSpriteDefinition( const BranchingSpriteDefinition& other ) :
	SpriteDefinition( other )
{
	m_sprites = other.m_sprites;
}

BranchingSpriteDefinition::~BranchingSpriteDefinition()
{
}

Randomness* BranchingSpriteDefinition::createRandomness( QStringList materialIDs )
{
	BranchRandomness* r = new BranchRandomness();
	for ( int i = 0; i < m_sprites.size(); i++ )
	{
		Randomness* rSub = m_sprites.values().at( i )->createRandomness( materialIDs );
		r->m_randomness.insert( i, rSub );
	}
	return r;
}

/******************************** SeasonSpriteDefinition  ********************************************/

SeasonSpriteDefinition::SeasonSpriteDefinition( SDID sID ) :
	BranchingSpriteDefinition( sID )
{
}

SeasonSpriteDefinition::SeasonSpriteDefinition( const SeasonSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
}

SeasonSpriteDefinition::~SeasonSpriteDefinition() 
{
}

Sprite* SeasonSpriteDefinition::createSprite( QStringList materialSID, Randomness* random )
{
	SpriteSeasons* ss = new SpriteSeasons;
	for ( auto key : m_sprites.keys() )
	{
		ss->m_sprites.insert( key, m_sprites.value( key )->createSprite( materialSID, random ) );
	}
	return ss;
}

/******************************** RotationSpriteDefinition  ********************************************/

RotationSpriteDefinition::RotationSpriteDefinition( SDID sID ) :
	BranchingSpriteDefinition( sID )
{
}

RotationSpriteDefinition::RotationSpriteDefinition( const RotationSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
}

RotationSpriteDefinition::~RotationSpriteDefinition()
{
}

Sprite* RotationSpriteDefinition::createSprite( QStringList materialSID, Randomness* random )
{
	SpriteRotations* sr = new SpriteRotations;

	sr->m_sprites.push_back( m_sprites.value( "FR" )->createSprite( materialSID, random ) );
	sr->m_sprites.push_back( m_sprites.value( "FL" )->createSprite( materialSID, random ) );
	sr->m_sprites.push_back( m_sprites.value( "BL" )->createSprite( materialSID, random ) );
	sr->m_sprites.push_back( m_sprites.value( "BR" )->createSprite( materialSID, random ) );

	return sr;
}


/******************************** FramesSpriteDefinition  ********************************************/

FramesSpriteDefinition::FramesSpriteDefinition( SDID sID ) :
	BranchingSpriteDefinition( sID )
{
}

FramesSpriteDefinition::FramesSpriteDefinition( const FramesSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
}

FramesSpriteDefinition::~FramesSpriteDefinition()
{
}

Sprite* FramesSpriteDefinition::createSprite( QStringList materialSID, Randomness* random )
{
	SpriteFrames* sf = new SpriteFrames;
	for ( auto child : m_sprites )
	{
		sf->m_sprites.push_back( child->createSprite( materialSID, random ) );
	}
	return sf;
}


/******************************** MaterialSpriteDefinition  ********************************************/

MaterialSpriteDefinition::MaterialSpriteDefinition( SDID sID, int position ) :
	BranchingSpriteDefinition( sID )
{
	m_type = "ByMaterials";
	m_position = position;
}

MaterialSpriteDefinition::MaterialSpriteDefinition( const MaterialSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
}

MaterialSpriteDefinition::~MaterialSpriteDefinition()
{
}

Sprite* MaterialSpriteDefinition::createSprite( QStringList materialIDs, Randomness* random )
{
	QString material = materialIDs.at( m_position );
	return m_sprites.value( material )->createSprite( materialIDs, random );
}

Randomness* MaterialSpriteDefinition::createRandomness( QStringList materialIDs )
{
	QString material = materialIDs.at( m_position );
	return m_sprites.value( material )->createRandomness( materialIDs );
}

/******************************** TypeSpriteDefinition  ********************************************/

TypeSpriteDefinition::TypeSpriteDefinition( SDID sID, int position, QMap<QString, QString> materialTypes ) :
	BranchingSpriteDefinition( sID )
{
	m_type     = "ByTypes";
	m_position = position;
	m_materialTypes = materialTypes;
}

TypeSpriteDefinition::TypeSpriteDefinition( const TypeSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
}

TypeSpriteDefinition::~TypeSpriteDefinition()
{
}

Sprite* TypeSpriteDefinition::createSprite( QStringList materialIDs, Randomness* random )
{
	QString material = materialIDs.at( m_position );
	QString type     = m_materialTypes.value( material );
	return m_sprites.value( type )->createSprite( materialIDs, random );
}

Randomness* TypeSpriteDefinition::createRandomness( QStringList materialIDs )
{
	QString material = materialIDs.at( m_position );
	QString type     = m_materialTypes.value( material );
	return m_sprites.value( type )->createRandomness( materialIDs );
}


/******************************** CombineSpriteDefinition  ********************************************/

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
		s->combine( s2, "", 0, 0 ); //TODO new combine
	}
	return s;
}

Randomness* CombineSpriteDefinition::createRandomness( QStringList materialIDs )
{
	BranchRandomness* r = new BranchRandomness();
	for ( int i = 0; i < m_sprites.size(); i++ )
	{
		Randomness* rSub = m_sprites.at( i )->createRandomness( materialIDs );
		r->m_randomness.insert( i, rSub );
	}
	return r;
}



/******************************** RandomSpriteDefinition  ********************************************/

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
	int randomNumber    = random->getRandom();
	Randomness* randomS = random->getRandomness( randomNumber );
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
			Randomness* subRand = m_sprites.at( i )->createRandomness( materialIDs );
			return new RandomRandomness( i, subRand );
		}
	}
}

/******************************** EffectSpriteDefinition  ********************************************/

LinearSpriteDefinition::LinearSpriteDefinition( SDID sID, SpriteDefinition* spriteDef ) :
	SpriteDefinition( sID )
{
	m_spriteDef = spriteDef;
}

LinearSpriteDefinition::LinearSpriteDefinition( const LinearSpriteDefinition& other ) :
	SpriteDefinition( other )
{
}

LinearSpriteDefinition::~LinearSpriteDefinition()
{
}

Randomness* LinearSpriteDefinition::createRandomness( QStringList materialIDs )
{
	return m_spriteDef->createRandomness( materialIDs );
}


/******************************** TintSpriteDefinition  ********************************************/

TintSpriteDefinition::TintSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, QString tint ) :
	LinearSpriteDefinition( sID, spriteDef )
{
	m_tint     = tint;
	m_material = -1;
}

TintSpriteDefinition::TintSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, int material ) :
	LinearSpriteDefinition( sID, spriteDef )
{
	m_tint     = "Material";
	m_material = material;
}

TintSpriteDefinition::TintSpriteDefinition( const TintSpriteDefinition& other ) :
	LinearSpriteDefinition( other )
{
}

TintSpriteDefinition::~TintSpriteDefinition()
{
}

Sprite* TintSpriteDefinition::createSprite( QStringList materialIDs, Randomness* random )
{
	Sprite* sprite = m_spriteDef->createSprite( materialIDs, random );
	if ( m_material >= 0 ) {
		QString material = materialIDs.at( m_material );
		sprite->applyTint(m_tint,material);
	} else {
		sprite->applyTint( m_tint, NULL );
	}
	return sprite;
}


/******************************** EffectSpriteDefinition  ********************************************/

EffectSpriteDefinition::EffectSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, QString effect ) :
	LinearSpriteDefinition( sID, spriteDef )
{
	m_effect = effect;
}

EffectSpriteDefinition::EffectSpriteDefinition( const EffectSpriteDefinition& other ) :
	LinearSpriteDefinition( other )
{
	m_effect = other.m_effect;
}

EffectSpriteDefinition::~EffectSpriteDefinition()
{
}

Sprite* EffectSpriteDefinition::createSprite( QStringList materialIDs, Randomness* random )
{
	Sprite* sprite = m_spriteDef->createSprite( materialIDs, random );
	sprite->applyEffect( m_effect );
	return sprite;
}
