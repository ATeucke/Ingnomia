
#include "../base/db.h"
#include "../gfx/spriteDefinition.h"

#include <QDebug>
#include <QPainter>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

SpriteDefinition::SpriteDefinition( SDID sID )
{
	m_sID = sID;
}

SpriteDefinition::SpriteDefinition( const SpriteDefinition& other )
{
	m_sID         = other.m_sID;
	m_type        = other.m_type;
}

SpriteDefinition::~SpriteDefinition()
{
}

QJsonObject SpriteDefinition::toJson()
{
	QJsonObject json;
	json.insert( "1_ID", m_sID );
	json.insert( "2_Type", m_type );

	return json;
}

QString SpriteDefinition::toString()
{
	QJsonDocument json = QJsonDocument( toJson() );
	return json.toJson( QJsonDocument::Indented );
}

void SpriteDefinition::replaceVariable( QString replace, QString with )
{
}

/******************************** BaseSpriteDefinition  ********************************************/

BaseSpriteDefinition::BaseSpriteDefinition( SDID sID, QString tilesheet, QString sourcerectangle, QPixmap pm ) :
	SpriteDefinition(sID)
{
	m_type            = "BaseSprite";
	m_tilesheet       = tilesheet;
	m_sourcerectangle = sourcerectangle;
	m_pixmap    = pm;
}

BaseSpriteDefinition::BaseSpriteDefinition( const BaseSpriteDefinition& other ) :
	SpriteDefinition( other )
{
	m_pixmap    = other.m_pixmap;
	m_offset    = other.m_offset;
	m_tilesheet       = other.m_tilesheet;
	m_sourcerectangle = other.m_sourcerectangle;
}

BaseSpriteDefinition::~BaseSpriteDefinition()
{
}

SpriteDefinition* BaseSpriteDefinition::copy()
{
	return new BaseSpriteDefinition(*this);
}

Sprite* BaseSpriteDefinition::createSprite( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	QPixmap pm           = m_pixmap;
	SpritePixmap* sprite = new SpritePixmap( pm, "0 0" );
	return sprite;
}

QJsonObject BaseSpriteDefinition::toJson()
{
	QJsonObject json = SpriteDefinition::toJson();
	json.insert( "3_Tilesheet", QJsonValue(m_tilesheet) );
	json.insert( "4_SourceRectangle", QJsonValue( m_sourcerectangle) );
	if (m_offset != "0 0")
		json.insert( "5_Offset", QJsonValue(m_offset) );

	return json;
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
	for ( auto key : other.m_sprites.keys() )
		m_sprites[key] = other.m_sprites[key]->copy();
	m_variable = other.m_variable;
}

BranchingSpriteDefinition::~BranchingSpriteDefinition()
{
}

void BranchingSpriteDefinition::add( QString key, SpriteDefinition* spriteDef )
{
	m_sprites.insert( key, spriteDef );
}

QJsonObject BranchingSpriteDefinition::toJson()
{
	QJsonObject json = SpriteDefinition::toJson();
	json.insert( "3_Var", m_variable );
	for ( auto key : m_sprites.keys() )
		json.insert( key, m_sprites.value(key)->toJson() );

	return  json ;
}

void BranchingSpriteDefinition::replaceVariable( QString replace, QString with )
{
	if ( m_variable == replace )
		m_variable = with;
	for ( auto sprite : m_sprites )
		sprite->replaceVariable( replace, with );
}

/******************************** SeasonSpriteDefinition  ********************************************/

SeasonSpriteDefinition::SeasonSpriteDefinition( SDID sID, QStringList seasons ) :
	BranchingSpriteDefinition( sID, "Season" )
{
	m_type = "SeasonSprite";
	m_seasons = seasons;
}

SeasonSpriteDefinition::SeasonSpriteDefinition( const SeasonSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
	m_seasons = other.m_seasons;
}

SeasonSpriteDefinition::~SeasonSpriteDefinition() 
{
}

SpriteDefinition* SeasonSpriteDefinition::copy()
{
	return new SeasonSpriteDefinition( *this );
}

Sprite* SeasonSpriteDefinition::createSprite( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	SpriteSeasons* ss = new SpriteSeasons;
	for ( auto key : m_sprites.keys() )
	{
		ss->m_sprites.insert( key, m_sprites.value( key )->createSprite( parameters, random ) );
	}
	return ss;
}

QJsonObject SeasonSpriteDefinition::toJson()
{
	QJsonObject json = SpriteDefinition::toJson();
	for ( auto key : m_sprites.keys() )
		json.insert( key, m_sprites.value( key )->toJson() );

	return json;
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

SpriteDefinition* RotationSpriteDefinition::copy()
{
	return new RotationSpriteDefinition( *this );
}

Sprite* RotationSpriteDefinition::createSprite( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	SpriteRotations* sr = new SpriteRotations;
	sr->m_sprites.push_back( m_sprites.value( "FR" )->createSprite( parameters, random ) );
	sr->m_sprites.push_back( m_sprites.value( "FL" )->createSprite( parameters, random ) );
	sr->m_sprites.push_back( m_sprites.value( "BL" )->createSprite( parameters, random ) );
	sr->m_sprites.push_back( m_sprites.value( "BR" )->createSprite( parameters, random ) );

	return sr;
}

QJsonObject RotationSpriteDefinition::toJson()
{
	QJsonObject json = SpriteDefinition::toJson();
	for ( auto key : m_sprites.keys() )
		json.insert( key, m_sprites.value( key )->toJson() );

	return json;
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

SpriteDefinition* FramesSpriteDefinition::copy()
{
	return new FramesSpriteDefinition( *this );
}

Sprite* FramesSpriteDefinition::createSprite( QMap<QString, QString> parameters, QMap<QString, int> random )
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

SpriteDefinition* MaterialSpriteDefinition::copy()
{
	return new MaterialSpriteDefinition( *this );
}

Sprite* MaterialSpriteDefinition::createSprite( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	QString material = parameters.value(m_variable);
	return m_sprites.value( material )->createSprite( parameters, random );
}

/******************************** TypeSpriteDefinition  ********************************************/

TypeSpriteDefinition::TypeSpriteDefinition( SDID sID, QString variable, QHash<QString, QString> materialTypes ) :
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

SpriteDefinition* TypeSpriteDefinition::copy()
{
	return new TypeSpriteDefinition( *this );
}

Sprite* TypeSpriteDefinition::createSprite( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	QString material = parameters.value( m_variable );
	QString type     = m_materialTypes.value( material );
	return m_sprites.value( type )->createSprite( parameters, random );
}


/******************************** CombineSpriteDefinition  ********************************************/

CombineSpriteDefinition::CombineSpriteDefinition( SDID sID, QStringList seasons ) :
	BranchingSpriteDefinition( sID , "")
{
	m_type = "Combine";
	m_seasons = seasons;
}

CombineSpriteDefinition::CombineSpriteDefinition( const CombineSpriteDefinition& other ) :
	BranchingSpriteDefinition( other )
{
	m_seasons = other.m_seasons;
	for ( auto sprite : other.m_sprites )
		m_sprites.append( sprite->copy());
}

CombineSpriteDefinition::~CombineSpriteDefinition()
{
}

SpriteDefinition* CombineSpriteDefinition::copy()
{
	return new CombineSpriteDefinition( *this );
}

Sprite* CombineSpriteDefinition::createSprite( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	Sprite* s = m_sprites.at( 0 )->createSprite( parameters, random );

	for ( int i = 1; i < m_sprites.size(); ++i )
	{
		Sprite* s2 = m_sprites.at( i )->createSprite( parameters, random );
		//s->combine( s2, "", 0, 0 ); //TODO new combine
		for ( auto season : m_seasons )
		{
			s->combine( s2, season, 0, 0 );
			s->combine( s2, season, 1, 0 );
			s->combine( s2, season, 2, 0 );
			s->combine( s2, season, 3, 0 );

			s->combine( s2, season, 0, 1 );
			s->combine( s2, season, 1, 1 );
			s->combine( s2, season, 2, 1 );
			s->combine( s2, season, 3, 1 );

			s->combine( s2, season, 0, 2 );
			s->combine( s2, season, 1, 2 );
			s->combine( s2, season, 2, 2 );
			s->combine( s2, season, 3, 2 );

			s->combine( s2, season, 0, 3 );
			s->combine( s2, season, 1, 3 );
			s->combine( s2, season, 2, 3 );
			s->combine( s2, season, 3, 3 );
		}
	}
	return s;
}

void CombineSpriteDefinition::add( QString key, SpriteDefinition* spriteDef )
{
	m_sprites.append( spriteDef );
}

QJsonObject CombineSpriteDefinition::toJson()
{
	QJsonArray arr;
	for ( auto sd : m_sprites )
		arr.append( sd->toJson() );

	QJsonObject json = SpriteDefinition::toJson();
	json.insert( "Sprites", arr );
	return json;
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
	for ( auto sprite : other.m_sprites )
		m_sprites.append( sprite->copy() );
} 

RandomSpriteDefinition::~RandomSpriteDefinition()
{
}

SpriteDefinition* RandomSpriteDefinition::copy()
{
	return new RandomSpriteDefinition( *this );
}

Sprite* RandomSpriteDefinition::createSprite( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	int randomNumber    = random.value( m_variable );
	int ran          = m_sum > 0 ? randomNumber % m_sum : randomNumber % m_weights.size();
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
	int wheight = key.toInt();
	m_sum += wheight;
	m_weights.append( wheight );
	m_sprites.append( spriteDef );
}

QJsonObject RandomSpriteDefinition::toJson()
{
	QJsonObject json = SpriteDefinition::toJson();

	json.insert( "3_Var", m_variable);
	if ( m_sum > 0)
	{
		QJsonArray weights;
		for ( auto w : m_weights )
			weights.append( w );
		json.insert( "4_Weights", weights );
	}

	QJsonArray arr;
	for ( auto sd : m_sprites )
		arr.append( sd->toJson() );
	json.insert( "Sprites", arr );


	return json;
}


void RandomSpriteDefinition::replaceVariable( QString replace, QString with )
{
	if ( m_variable == replace )
		m_variable = with;
	for ( auto sprite : m_sprites )
		sprite->replaceVariable( replace, with );
}

/******************************** LinearSpriteDefinition  ********************************************/

LinearSpriteDefinition::LinearSpriteDefinition( SDID sID, SpriteDefinition* spriteDef ) :
	SpriteDefinition( sID )
{
	m_spriteDef = spriteDef;
}

LinearSpriteDefinition::LinearSpriteDefinition( const LinearSpriteDefinition& other ) :
	SpriteDefinition( other )
{
	m_spriteDef = other.m_spriteDef->copy();
}

LinearSpriteDefinition::~LinearSpriteDefinition()
{
}

QJsonObject LinearSpriteDefinition::toJson()
{
	return m_spriteDef->toJson();
}

void LinearSpriteDefinition::replaceVariable( QString replace, QString with )
{
	m_spriteDef->replaceVariable( replace, with );
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

SpriteDefinition* TintSpriteDefinition::copy()
{
	return new TintSpriteDefinition( *this );
}

Sprite* TintSpriteDefinition::createSprite( QMap<QString, QString> parameters, QMap<QString, int> random )
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

QJsonObject TintSpriteDefinition::toJson()
{
	QJsonObject json = LinearSpriteDefinition::toJson();
	json.insert( "5_Tint", m_variable );
	return json;
}


void TintSpriteDefinition::replaceVariable( QString replace, QString with )
{
	if ( m_variable == replace )
		m_variable = with;
	m_spriteDef->replaceVariable( replace, with );
}


/******************************** EffectSpriteDefinition  ********************************************/

EffectSpriteDefinition::EffectSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, QString effect ) :
	LinearSpriteDefinition( sID, spriteDef )
{
	m_effect = effect;
	m_type   = "Effect";
}

EffectSpriteDefinition::EffectSpriteDefinition( const EffectSpriteDefinition& other ) :
	LinearSpriteDefinition( other )
{
	m_effect = other.m_effect;
}

EffectSpriteDefinition::~EffectSpriteDefinition()
{
}

SpriteDefinition* EffectSpriteDefinition::copy()
{
	return new EffectSpriteDefinition( *this );
}

Sprite* EffectSpriteDefinition::createSprite( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	Sprite* sprite = m_spriteDef->createSprite( parameters, random );
	sprite->applyEffect( m_effect );
	return sprite;
}

QJsonObject EffectSpriteDefinition::toJson()
{
	QJsonObject json = LinearSpriteDefinition::toJson();
	json.insert( "5_Effect", m_effect );
	return json;
}
