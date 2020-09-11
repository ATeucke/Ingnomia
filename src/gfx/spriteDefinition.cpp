
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
	m_sID         = other.m_sID;
	m_type        = other.m_type;
}

SpriteDefinition::~SpriteDefinition()
{
}

/******************************** BaseSpriteDefinition  ********************************************/

BaseSpriteDefinition::BaseSpriteDefinition( SDID sID, QString tilesheet, QString offset, QPixmap pm ) :
	SpriteDefinition(sID)
{
	m_offset   = offset;
	m_tilesheet = tilesheet;
	m_pixmap    = pm;
}

BaseSpriteDefinition::BaseSpriteDefinition( const BaseSpriteDefinition& other ) :
	SpriteDefinition( other )
{
	m_pixmap    = other.m_pixmap;
	m_offset    = other.m_offset;
	m_tilesheet = other.m_tilesheet;
}

BaseSpriteDefinition::~BaseSpriteDefinition()
{
}

Sprite* BaseSpriteDefinition::createSprite( QMap<QString,QString> parameters, QMap<QString,int> random )
{
	QPixmap pm           = m_pixmap;
	SpritePixmap* sprite = new SpritePixmap( pm, "0 0" );
	return sprite;
}

/******************************** BranchingSpriteDefinition  ********************************************/

BranchingSpriteDefinition::BranchingSpriteDefinition( SDID sID, QString variable ) :
	SpriteDefinition( sID )
{
	m_variable = variable;
}

BranchingSpriteDefinition::BranchingSpriteDefinition( const BranchingSpriteDefinition& other ) :
	SpriteDefinition( other )
{
	m_sprites  = other.m_sprites;
	m_variable = other.m_variable;
}

BranchingSpriteDefinition::~BranchingSpriteDefinition()
{
}

void BranchingSpriteDefinition::add( QString key, SpriteDefinition* spriteDef )
{
	m_sprites.insert( key, spriteDef );
}

/******************************** SeasonSpriteDefinition  ********************************************/

SeasonSpriteDefinition::SeasonSpriteDefinition( SDID sID ) :
	BranchingSpriteDefinition( sID, "Season" )
{
	m_type = "SeasonSprite";
}

SeasonSpriteDefinition::SeasonSpriteDefinition( const SeasonSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
}

SeasonSpriteDefinition::~SeasonSpriteDefinition() 
{
}

Sprite* SeasonSpriteDefinition::createSprite( QMap<QString,QString> parameters, QMap<QString,int> random )
{
	SpriteSeasons* ss = new SpriteSeasons;
	for ( auto key : m_sprites.keys() )
	{
		ss->m_sprites.insert( key, m_sprites.value( key )->createSprite( parameters, random ) );
	}
	return ss;
}

/******************************** RotationSpriteDefinition  ********************************************/

RotationSpriteDefinition::RotationSpriteDefinition( SDID sID ) :
	BranchingSpriteDefinition( sID, "Rotation" )
{
	m_type = "RotationSprite";
}

RotationSpriteDefinition::RotationSpriteDefinition( const RotationSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
}

RotationSpriteDefinition::~RotationSpriteDefinition()
{
}

Sprite* RotationSpriteDefinition::createSprite( QMap<QString,QString> parameters, QMap<QString,int> random )
{
	SpriteRotations* sr = new SpriteRotations;
	sr->m_sprites.push_back( m_sprites.value( "FR" )->createSprite( parameters, random ) );
	sr->m_sprites.push_back( m_sprites.value( "FL" )->createSprite( parameters, random ) );
	sr->m_sprites.push_back( m_sprites.value( "BL" )->createSprite( parameters, random ) );
	sr->m_sprites.push_back( m_sprites.value( "BR" )->createSprite( parameters, random ) );

	return sr;
}


/******************************** FramesSpriteDefinition  ********************************************/

FramesSpriteDefinition::FramesSpriteDefinition( SDID sID ) :
	BranchingSpriteDefinition( sID, "Frame" )
{
	m_type = "FramesSprite";
}

FramesSpriteDefinition::FramesSpriteDefinition( const FramesSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
}

FramesSpriteDefinition::~FramesSpriteDefinition()
{
}

Sprite* FramesSpriteDefinition::createSprite( QMap<QString,QString> parameters, QMap<QString,int> random )
{
	SpriteFrames* sf = new SpriteFrames;
	for ( auto child : m_sprites )
	{
		sf->m_sprites.push_back( child->createSprite( parameters, random ) );
	}
	return sf;
}


/******************************** MaterialSpriteDefinition  ********************************************/

MaterialSpriteDefinition::MaterialSpriteDefinition( SDID sID, QString variable ) :
	BranchingSpriteDefinition( sID, variable )
{
	m_type = "ByMaterials";
}

MaterialSpriteDefinition::MaterialSpriteDefinition( const MaterialSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
}

MaterialSpriteDefinition::~MaterialSpriteDefinition()
{
}

Sprite* MaterialSpriteDefinition::createSprite( QMap<QString,QString> parameters, QMap<QString,int> random )
{
	QString material = parameters.value(m_variable);
	return m_sprites.value( material )->createSprite( parameters, random );
}

/******************************** TypeSpriteDefinition  ********************************************/

TypeSpriteDefinition::TypeSpriteDefinition( SDID sID, QString variable, QMap<QString, QString> materialTypes ) :
	BranchingSpriteDefinition( sID, variable )
{
	m_type     = "ByTypes";
	m_materialTypes = materialTypes;
}

TypeSpriteDefinition::TypeSpriteDefinition( const TypeSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
	m_materialTypes = other.m_materialTypes;
}

TypeSpriteDefinition::~TypeSpriteDefinition()
{
}

Sprite* TypeSpriteDefinition::createSprite( QMap<QString,QString> parameters, QMap<QString,int> random )
{
	QString material = parameters.value( m_variable );
	QString type     = m_materialTypes.value( material );
	return m_sprites.value( type )->createSprite( parameters, random );
}


/******************************** CombineSpriteDefinition  ********************************************/

CombineSpriteDefinition::CombineSpriteDefinition( SDID sID ) :
	BranchingSpriteDefinition( sID , "")
{
	m_type = "Combine";
}

CombineSpriteDefinition::CombineSpriteDefinition( const CombineSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
}

CombineSpriteDefinition::~CombineSpriteDefinition()
{
}

Sprite* CombineSpriteDefinition::createSprite( QMap<QString,QString> parameters, QMap<QString,int> random )
{
	Sprite* s = m_sprites.at( 0 )->createSprite( parameters, random );

	for ( int i = 1; i < m_sprites.size(); ++i )
	{
		Sprite* s2 = m_sprites.at( i )->createSprite( parameters, random );
		s->combine( s2, "", 0, 0 ); //TODO new combine
	}
	return s;
}

void CombineSpriteDefinition::add( QString key, SpriteDefinition* spriteDef )
{
	m_sprites.append( spriteDef );
}

/******************************** RandomSpriteDefinition  ********************************************/

RandomSpriteDefinition::RandomSpriteDefinition( SDID sID, QString variable ) :
	BranchingSpriteDefinition( sID, variable )
{
	m_type     = "Random";
}

RandomSpriteDefinition::RandomSpriteDefinition( const RandomSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
	m_weights  = other.m_weights;
	m_sprites  = other.m_sprites;
} 

RandomSpriteDefinition::~RandomSpriteDefinition()
{
}

Sprite* RandomSpriteDefinition::createSprite( QMap<QString,QString> parameters, QMap<QString,int> random )
{
	int randomNumber    = random.value( m_variable );
	int ran          = randomNumber % m_sum;
	int total        = 0;
	for ( int i = 0; i < m_weights.size(); ++i )
	{
		total += m_weights[i];
		if ( ran < total )
		{
			return m_sprites.at( i )->createSprite( parameters, random );
		}
	}
	return m_sprites.at( 0 )->createSprite( parameters, random );
}

void RandomSpriteDefinition::add( QString key, SpriteDefinition* spriteDef )
{
	m_weights.append( key.toInt() );
	m_sprites.append( spriteDef );
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
	m_spriteDef = other.m_spriteDef;
}

LinearSpriteDefinition::~LinearSpriteDefinition()
{
}


/******************************** TintSpriteDefinition  ********************************************/

TintSpriteDefinition::TintSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, QString variable ) :
	LinearSpriteDefinition( sID, spriteDef )
{
	m_type     = "Tint";
	m_variable = variable;
}

TintSpriteDefinition::TintSpriteDefinition( const TintSpriteDefinition& other ) :
	LinearSpriteDefinition( other )
{
	m_variable = other.m_variable;
}

TintSpriteDefinition::~TintSpriteDefinition()
{
}

Sprite* TintSpriteDefinition::createSprite( QMap<QString,QString> parameters, QMap<QString,int> random )
{
	Sprite* sprite = m_spriteDef->createSprite( parameters, random );
	if ( m_variable.contains(" ") ) 
	{ 
		/* m_variable is an RGB value */
		sprite->applyTint( m_variable, NULL );
	}
	else if ( parameters.contains( m_variable ) )
	{
		/* m_variable is a variable */
		QString material = parameters.value( m_variable );
		sprite->applyTint( "Material", material );
	}
	else
	{
		/* m_variable is a material */
		sprite->applyTint( "Material", m_variable );
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

Sprite* EffectSpriteDefinition::createSprite( QMap<QString,QString> parameters, QMap<QString,int> random )
{
	Sprite* sprite = m_spriteDef->createSprite( parameters, random );
	sprite->applyEffect( m_effect );
	return sprite;
}
