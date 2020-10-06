#include "spriteDefFactory.h"

#include "../base/config.h"
#include "../base/db.h"
#include "../base/io.h"
#include "exprtk.hpp"

#include <QDebug>
#include <QElapsedTimer>
#include <QHash>
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include "jsonSpriteDefFactory.h"




JsonSpriteDefFactory::JsonSpriteDefFactory()
{
}

JsonSpriteDefFactory::~JsonSpriteDefFactory()
{
}

bool JsonSpriteDefFactory::init()
{
	loadCaches(); 
	loadSpriteDefinitions();

	for ( auto spriteDef : m_jsonDefs )
	{
		QString id = spriteDef.value( "ID" ).toString();
		bool debug = false;
		if ( spriteDef.contains( "Debug" ) && spriteDef.value( "Debug" ).toString() == "true" )
			debug = true;
		if ( spriteDef.value( "Type" ) != "Template")
			m_cachedSpriteDefs.insert( id, new CachedSpriteDefinition( id, fromJson( spriteDef ), debug ) );
	}
	saveToFile("SpriteDefinitionsFromJson");
  
	for ( auto spriteDef : m_cachedSpriteDefs )
	{
		QMap<QString, QString> parameters;
		createSprite( spriteDef, parameters );
	}

	return true;  
}

SpriteDefinition* JsonSpriteDefFactory::fromJson( QJsonObject jsonSpriteDef )
{
	QString id   = jsonSpriteDef.value( "ID" ).toString();
	QString type = jsonSpriteDef.value( "Type" ).toString();
	SpriteDefinition* spriteDef = NULL;
	if ( "Base" == type )
		spriteDef = createBaseSpriteDef( jsonSpriteDef );
	else if ( "ByTypes" == type )
		spriteDef = createTypeSpriteDef( jsonSpriteDef );
	else if ( "ByMaterials" == type )
		spriteDef = createMaterialSpriteDef( jsonSpriteDef );
	else if ( "Combine" == type )
		spriteDef = createCombineSpriteDef( jsonSpriteDef );
	else if ( "Frames" == type )
		spriteDef = createFramesSpriteDef( jsonSpriteDef );
	else if ( "Rotation" == type )
		spriteDef = createRotationSpriteDef( jsonSpriteDef );
	else if ( "Season" == type )
		spriteDef = createSeasonSpriteDef( jsonSpriteDef );
	else if ( "Random" == type )
		spriteDef = createRandomSpriteDef( jsonSpriteDef );
	else if ( "ApplyTemplate" == type )
		spriteDef = createApplyTemplateSpriteDef( jsonSpriteDef );
	else
		qDebug() << "***ERROR*** unknown type for " << jsonSpriteDef ;


	if ( jsonSpriteDef.contains( "Effect" ) )
	{
		QString effect = jsonSpriteDef.value( "Effect" ).toString();
		spriteDef      = new EffectSpriteDefinition( id + effect, spriteDef, effect );
	}


	if ( jsonSpriteDef.contains( "Tint" ) )
	{
		QString tint = jsonSpriteDef.value( "Tint" ).toString();
		spriteDef    = new TintSpriteDefinition( id + "_Tint", spriteDef, tint );
	}

	return spriteDef;
}

SpriteDefinition* JsonSpriteDefFactory::createApplyTemplateSpriteDef( QJsonObject jsonSpriteDef )
{

	QString id         = jsonSpriteDef.value( "ID" ).toString();
	QJsonArray args = jsonSpriteDef.value( "Arguments" ).toArray();

	QStringList arguments;
	for ( auto var : args )
		arguments.append( var.toString() );

	QString templateID = jsonSpriteDef.value( "Template" ).toString();
	if ( !m_templates.contains( templateID ) )
	{
		qDebug() << "***ERROR*** no template with id " << templateID;
		return NULL; 
	}
	SpriteDefinitionTemplate* sp = m_templates.value( templateID );

	if ( sp->m_variables.size() != arguments.size() )
	{
		qDebug() << "***ERROR*** no. of arguments of template does not match " << jsonSpriteDef;
		return NULL;  
	}

	QJsonObject defJson = QJsonObject( sp->m_template);
	
	for ( int i = 0; i < arguments.size(); i++ )
	{
		defJson = jsonReplace( defJson, sp->m_variables[i], arguments[i] );
		//qDebug() << "arguments replaced  " << defJson;
	}
	//qDebug() << "arguments replaced  " << defJson;

	return fromJson( defJson );
}

void JsonSpriteDefFactory::createSpriteDefTemplate( QJsonObject jsonSpriteDef )
{
	QString id                 = jsonSpriteDef.value( "ID" ).toString();
	QJsonArray vars            = jsonSpriteDef.value( "Vars" ).toArray();
	QJsonObject sprite         = jsonSpriteDef.value( "Sprite" ).toObject();

	QStringList variables;
	for ( auto var : vars )
		variables.append( var.toString() );

	m_templates.insert(id, new SpriteDefinitionTemplate( id, sprite, variables ));

}


SpriteDefinition* JsonSpriteDefFactory::createRandomSpriteDef( QJsonObject jsonSpriteDef )
{

	QString id                 = jsonSpriteDef.value( "ID" ).toString();
	QString var                = jsonSpriteDef.value( "Var" ).toString();
	RandomSpriteDefinition* sp = new RandomSpriteDefinition( id, var );

	auto sprites = jsonSpriteDef.value( "Sprites" ).toArray();
	QJsonArray wheigts;
	if ( jsonSpriteDef.contains( "Wheights" ) )
		 wheigts = jsonSpriteDef.value( "Wheights" ).toArray();

	for ( int i = 0; i < sprites.size(); i++ )
	{
		if ( !wheigts.isEmpty() )
			sp->add( wheigts.at(i).toString(), fromJson( sprites.at(i).toObject() ) );
		else
			sp->add( "1", fromJson( sprites.at( i ).toObject() ) );
	}

	return sp;
}

SpriteDefinition* JsonSpriteDefFactory::createFramesSpriteDef( QJsonObject jsonSpriteDef )
{

	QString id                  = jsonSpriteDef.value( "ID" ).toString();
	FramesSpriteDefinition* sp = new FramesSpriteDefinition( id );

	for ( auto key : jsonSpriteDef.keys() )
	{
		if ( key.startsWith("F") )
			sp->add( key, fromJson( jsonSpriteDef.value( key ).toObject() ) );
	}

	return sp;
}


SpriteDefinition* JsonSpriteDefFactory::createCombineSpriteDef( QJsonObject jsonSpriteDef )
{

	QString id                   = jsonSpriteDef.value( "ID" ).toString();
	CombineSpriteDefinition* sp = new CombineSpriteDefinition( id , m_seasons);

	auto sprites = jsonSpriteDef.value("Sprites").toArray();
	int i        = 0;
	for ( auto combine : sprites )
	{
		sp->add( QString::number( i), fromJson( combine.toObject() ) );
		i++;
	}

	return sp;
}

SpriteDefinition* JsonSpriteDefFactory::createRotationSpriteDef( QJsonObject jsonSpriteDef )
{

	QString id = jsonSpriteDef.value( "ID" ).toString();
	RotationSpriteDefinition* sp = new RotationSpriteDefinition( id );
	
	for ( auto rot : m_rotations )
	{
		auto rotjson = jsonSpriteDef.value( rot ).toObject();
		auto rotDef  = fromJson( rotjson ); 
		sp->add( rot, rotDef );
	}

	return sp;
}

SpriteDefinition* JsonSpriteDefFactory::createSeasonSpriteDef( QJsonObject jsonSpriteDef )
{

	QString id                   = jsonSpriteDef.value( "ID" ).toString();
	SeasonSpriteDefinition* sp = new SeasonSpriteDefinition( id, m_seasons );

	for ( auto season : m_seasons )
		sp->add( season, fromJson( jsonSpriteDef.value( season ).toObject() ) );

	return sp;
}

SpriteDefinition* JsonSpriteDefFactory::createTypeSpriteDef( QJsonObject jsonSpriteDef )
{

	QString id               = jsonSpriteDef.value( "ID" ).toString();
	QString var              = jsonSpriteDef.value( "Var" ).toString();
	TypeSpriteDefinition* sp = new TypeSpriteDefinition( id, var, m_materialTypes );

	for ( auto key : jsonSpriteDef.keys() )
	{
		if ( m_materialTypes.values().contains( key ) )
			sp->add( key, fromJson( jsonSpriteDef.value( key ).toObject() ) );
	}
		
	return sp;
}

SpriteDefinition* JsonSpriteDefFactory::createMaterialSpriteDef( QJsonObject jsonSpriteDef )
{

	QString id               = jsonSpriteDef.value( "ID" ).toString();
	QString var              = jsonSpriteDef.value( "Var" ).toString();
	MaterialSpriteDefinition* sp = new MaterialSpriteDefinition( id, var );

	for ( auto key : jsonSpriteDef.keys() )
	{
		if ( m_materialTypes.contains( key ) )
		{
			auto matjson = jsonSpriteDef.value( key ).toObject();
			auto matDef  = fromJson( matjson );
			sp->add( key, matDef );
		}
	}

	return sp;
}

SpriteDefinition* JsonSpriteDefFactory::createBaseSpriteDef( QJsonObject jsonSpriteDef )
{
	QString id               = jsonSpriteDef.value( "ID" ).toString();
	QString tilesheet        = jsonSpriteDef.value( "Tilesheet" ).toString();
	QString sourceRectangle = jsonSpriteDef.value( "SourceRectangle" ).toString();

	QStringList rl = sourceRectangle.split( " " );
	for ( int i=0; i < rl.size(); i++ )
	{
		bool ok;
		rl[i].toInt(&ok);
		if ( !ok )
		{
			exprtk::expression<double> expression;
			exprtk::parser<double> parser;
			parser.compile( rl[i].toStdString(), expression );
			rl[i]= QString::number(expression.value());
		}
	}
	sourceRectangle = rl.join( " " );

	QPixmap pixmap           = loadTilesheet( tilesheet ); 
	QPixmap pm               = extractPixmap( id, pixmap, sourceRectangle );
	BaseSpriteDefinition* sp = new BaseSpriteDefinition( id, tilesheet, sourceRectangle, pm );

	if ( jsonSpriteDef.contains( "Offset" ) )
	{
		sp->m_offset = jsonSpriteDef.value( "Offset" ).toString();
	}

	return sp;
}

bool JsonSpriteDefFactory::loadSpriteDefinitions()
{
	const QString ID = "ID";

	QJsonDocument sd;
	bool ok = IO::loadFile( Config::getInstance().get( "dataPath" ).toString() + "/JSON/complexSpriteDefinitions.json", sd );
	if ( ok )
	{
		for ( auto def : sd.array() )
		{
			auto spriteDef = def.toObject();
			if ( spriteDef.contains( ID ) )
			{
				QString id = spriteDef.value( ID ).toString();
				if ( spriteDef.value( "Type" ).toString() == "Template" )
					createSpriteDefTemplate( spriteDef );
				else
					m_jsonDefs.insert( id, spriteDef );
			}
			else
				qDebug() << "***ERROR*** no id  for " << spriteDef;
		}
	}

	return true;
}

void JsonSpriteDefFactory::jsonReplace( QJsonValueRef json, QString before, QString after )
{

	if ( json.isString() )
	{
		QString string = json.toString();
		if ( string.contains( before ) )
		{
			const QString newString = string.replace( before, after );
			json                    = QJsonValue( newString );
		}
	}
	else if ( json.isObject() )
		json = jsonReplace( json.toObject(), before, after );
	else if ( json.isArray() )
		json = jsonReplace( json.toArray(), before, after );

}

QJsonObject JsonSpriteDefFactory::jsonReplace( QJsonObject json, QString before, QString after )
{
	for ( auto val : json )
		jsonReplace( val ,before, after);

	return json;
}

QJsonArray JsonSpriteDefFactory::jsonReplace( QJsonArray json, QString before, QString after )
{
	for ( auto val : json )
		jsonReplace( val, before, after );

	return json;
}



/******************************** SpriteDefinitionTemplate  ********************************************/

SpriteDefinitionTemplate::SpriteDefinitionTemplate( SDID sID, QJsonObject spriteDef, QList<QString> variables ) 
{
	m_sID       = sID;
	m_variables = variables;
	m_template  = spriteDef ;
}

SpriteDefinitionTemplate::SpriteDefinitionTemplate( const SpriteDefinitionTemplate& other ) 
{
	m_sID = other.m_sID;
	m_variables.append( other.m_variables );
	m_template = QJsonObject( other.m_template );
}

SpriteDefinitionTemplate::~SpriteDefinitionTemplate()
{
}

