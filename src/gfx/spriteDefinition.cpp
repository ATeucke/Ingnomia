
#include "../base/db.h"
#include "../gfx/spriteDefinition.h"

#include <QDebug>
#include <QPainter>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <numeric> 

using namespace std;

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


/******************************** CachedSpriteDefinition  ********************************************/

CachedSpriteDefinition::CachedSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, bool debug ) :
	SpriteDefinition( sID )
{
	m_type      = "ComplexSprite";
	m_spriteDef = spriteDef->copy();
	m_debug     = debug;

	//Normalize the random-variables  to R0,R1,...
	m_randomVariables = m_spriteDef->getRandomVariables();
	int i             = 0;
	for ( QString var : m_randomVariables.keys() )
	{
		m_spriteDef->replaceVariable( var, "R" + QString::number( i ) );
		i++;
	}
	m_randomVariables.~QMap();
	m_randomVariables = m_spriteDef->getRandomVariables();

	getTypes( &m_types );
	m_animFrames = m_spriteDef->getAnimFrames();
}

CachedSpriteDefinition::CachedSpriteDefinition( const CachedSpriteDefinition& other ) :
	SpriteDefinition( other )
{
	m_spriteDef = other.m_spriteDef->copy();
	m_randomVariables.insert( other.m_randomVariables );
	m_debug = other.m_debug;
}

CachedSpriteDefinition::~CachedSpriteDefinition()
{
	m_spriteDef->~SpriteDefinition();
	m_randomVariables.~QMap();
}

SpriteDefinition* CachedSpriteDefinition::copy()
{
	return &CachedSpriteDefinition( *this );
}

QPixmap& CachedSpriteDefinition::getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	return m_spriteDef->getPixmap( parameters, random );
}


QMap<QString, int> CachedSpriteDefinition::getRandomVariables()
{
	return m_randomVariables;
}

int CachedSpriteDefinition::getAnimFrames()
{
	return m_animFrames;
}

void CachedSpriteDefinition::getTypes( QSet<QString>* types )
{
	m_spriteDef->getTypes( types );
}

QJsonObject CachedSpriteDefinition::toJson()
{
	auto json = m_spriteDef->toJson();
	return json;
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

QPixmap& BaseSpriteDefinition::getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	//TODO add offset
	return m_pixmap;
}

QMap<QString, int> BaseSpriteDefinition::getRandomVariables()
{
	return QMap<QString, int>();
}

int BaseSpriteDefinition::getAnimFrames()
{
	return 1;
}

void BaseSpriteDefinition::getTypes( QSet<QString>* types )
{
	types->insert( m_type );
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

QPixmap& BranchingSpriteDefinition::getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	QString variable = parameters.value( m_variable );

	if ( m_sprites.contains( variable ) )
		return m_sprites.value( variable )->getPixmap( parameters, random );
	else
	{
		//TODO use default material
		qDebug() << "***WARN*** variable " << variable << " is not defined for " << this->m_sID;
		return m_sprites.first()->getPixmap( parameters, random );
	}
	return m_sprites.value( variable )->getPixmap( parameters, random );
}

QMap<QString, int> BranchingSpriteDefinition::getRandomVariables()
{
	QMap<QString, int> vars;
	for ( auto spriteDef : m_sprites )
		vars.insert( spriteDef->getRandomVariables() );

	return vars;
}

int BranchingSpriteDefinition::getAnimFrames()
{
	int acc = 1;
	for ( auto sd : m_sprites )
		acc = std::lcm( acc, sd->getAnimFrames());
	return acc;
}

void BranchingSpriteDefinition::getTypes( QSet<QString>* types )
{
	types->insert( m_type );
	for ( auto sd : m_sprites )
		sd->getTypes(types);
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

int FramesSpriteDefinition::getAnimFrames()
{
	return m_sprites.size();
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

QPixmap& TypeSpriteDefinition::getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	QString material = parameters.value( m_variable );
	QString type     = m_materialTypes.value( material );
	if ( m_sprites.contains( type ) )
		return m_sprites.value( type )->getPixmap( parameters, random );
	else
	{
		//TODO use default material
		qDebug() << "***WARN*** material " << material << " is not defined for " << this->m_sID;
		return m_sprites.first()->getPixmap( parameters, random );
	}
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
}

CombineSpriteDefinition::~CombineSpriteDefinition()
{
}

SpriteDefinition* CombineSpriteDefinition::copy()
{
	return new CombineSpriteDefinition( *this );
}

void CombineSpriteDefinition::add( QString key, SpriteDefinition* spriteDef )
{
	QString mapkey = QString( 'a' + m_sprites.size() );
	m_sprites.insert( mapkey, spriteDef );
}

QPixmap& CombineSpriteDefinition::getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	QPixmap pixmap ;
	QPainter painter( &pixmap );

	for ( auto iter = m_sprites.begin(); iter != m_sprites.end(); ++iter )
		painter.drawPixmap( 0, 0, iter.value()->getPixmap( parameters, random ) );

	return pixmap;
}

int CombineSpriteDefinition::getAnimFrames()
{
	return m_frames;
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
	m_weights.append(other.m_weights);
	m_sum     = other.m_sum;
} 

RandomSpriteDefinition::~RandomSpriteDefinition()
{
}

SpriteDefinition* RandomSpriteDefinition::copy()
{
	return new RandomSpriteDefinition( *this );
}

QPixmap& RandomSpriteDefinition::getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	int randomNumber    = random.value( m_variable );
	int ran          = m_sum > 0 ? randomNumber % m_sum : randomNumber % m_weights.size();
	int total        = 0;
	for ( int i = 0; i < m_weights.size(); ++i )
	{
		total += m_weights[i];
		if ( ran < total )
		{
			return m_sprites.values().at( i )->getPixmap( parameters, random );
		}
	}
	return m_sprites.values().at( 0 )->getPixmap( parameters, random );
}

void RandomSpriteDefinition::add( QString key, SpriteDefinition* spriteDef )
{
	int wheight = key.toInt();
	m_sum += wheight;
	m_weights.append( wheight );

	QString mapkey = QString ('a' + m_sprites.size());
	m_sprites.insert( mapkey, spriteDef );
}

QMap<QString, int> RandomSpriteDefinition::getRandomVariables()
{
	QMap<QString, int> vars;
	for ( auto spriteDef : m_sprites )
		vars.insert( spriteDef->getRandomVariables() );
	vars.insert( m_variable, m_sum );

	return vars;
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
	for ( auto sd : m_sprites.values() )
		arr.append( sd->toJson() );
	json.insert( "Sprites", arr );

	return json;
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

QMap<QString, int> LinearSpriteDefinition::getRandomVariables()
{
	return m_spriteDef->getRandomVariables();
}

int LinearSpriteDefinition::getAnimFrames()
{
	return m_spriteDef->getAnimFrames();
}

void LinearSpriteDefinition::getTypes( QSet<QString>* types )
{
	types->insert( m_type );
	m_spriteDef->getTypes( types );
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

QPixmap& TintSpriteDefinition::getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	QPixmap& pixmap = m_spriteDef->getPixmap( parameters, random );
	if ( m_variable.contains(" ") ) 
	{ 
		/* m_variable is an RGB value */
		QString tint = m_variable;
		return applyTint( pixmap, tint );
	}
	else if ( parameters.contains( m_variable ) )
	{
		/* m_variable is a variable */
		QString material = parameters.value( m_variable );
		QString tint     = DB::select( "Color", "Materials", material ).toString();
		return applyTint( pixmap, tint );
	}
	else
	{
		/* m_variable is a material */
		QString material =  m_variable ;
		QString tint     = DB::select( "Color", "Materials", material ).toString();
		return applyTint( pixmap, tint );
	}
}

QPixmap& TintSpriteDefinition::applyTint( QPixmap& pixmap, QString tint )
{

	QColor color;

	QList<QString> csl = tint.split( ' ' );
	if ( csl.size() == 4 )
		color = QColor( csl[0].toInt(), csl[1].toInt(), csl[2].toInt(), csl[3].toInt() );
	else
	{
		//qDebug() << "no tint" << materialSID << tint;
		color = QColor( 255, 255, 255, 255 );
	}
	auto opacity = color.alphaF();

	QImage img = pixmap.toImage();

	for ( int x = 0; x < img.size().width(); ++x )
	{
		for ( int y = 0; y < img.size().height(); ++y )
		{
			if ( y > 63 )
				qDebug() << "SpritePixmap::applyTint" << m_sID;
			QColor col = img.pixelColor( x, y );
			col.setRedF( qMin( 1., col.redF() * color.redF() ) );
			col.setGreenF( qMin( 1., col.greenF() * color.greenF() ) );
			col.setBlueF( qMin( 1., col.blueF() * color.blueF() ) );
			col.setAlphaF( qMin( 1., col.alphaF() * color.alphaF() ) );
			img.setPixelColor( x, y, col );
		}
	}
	QPixmap  tintedpixmap = QPixmap::fromImage( img );
	return tintedpixmap;
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

QPixmap& EffectSpriteDefinition::getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random )
{
	QPixmap& pixmap = m_spriteDef->getPixmap( parameters, random );
	return applyEffect( pixmap, m_effect );
}

QJsonObject EffectSpriteDefinition::toJson()
{
	QJsonObject json = LinearSpriteDefinition::toJson();
	json.insert( "5_Effect", m_effect );
	return json;
}


QPixmap& EffectSpriteDefinition::applyEffect( QPixmap& pixmap, QString effect )
{
	if ( effect == "FlipHorizontal" )
	{
		QImage img = pixmap.toImage();
		QPixmap flip = QPixmap::fromImage( img.mirrored( true, false ) );
		return flip;
	}
	else if ( effect == "Rot90" )
	{
		QImage img = pixmap.toImage();
		QImage tmp( 32, 32, QImage::Format::Format_RGBA8888 );
		for ( int y = 0; y < 32; ++y )
		{
			for ( int x = 0; x < 32; ++x )
			{
				tmp.setPixelColor( x, y, img.pixelColor( x, y + 16 ) );
			}
		}
		QPixmap pm = QPixmap::fromImage( tmp );
		pm         = pm.transformed( QTransform().rotate( 90 ) );
		tmp        = pm.toImage();
		for ( int y = 0; y < 32; ++y )
		{
			for ( int x = 0; x < 32; ++x )
			{
				img.setPixelColor( x, y + 16, tmp.pixelColor( x, y ) );
			}
		}
		QPixmap rot90 = QPixmap::fromImage( img );
		return rot90;
	}
}
