#include "spriteDefFactory.h"

#include "../base/config.h"
#include "../base/db.h"
#include "../base/io.h"

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
	m_rotations = {"BL","BR","FL","FR"};
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

	}

	return false;
}

SpriteDefinition* JsonSpriteDefFactory::fromJson( QJsonObject jsonSpriteDef )
{
	QString type = jsonSpriteDef.value( "Type" ).toString();
	SpriteDefinition* spriteDef = NULL;
	if ( "Base" == type )
		spriteDef = createBaseSpriteDef( jsonSpriteDef );
	else if ( "ByTypes" == type )
		spriteDef = createTypeSpriteDef( jsonSpriteDef );
	else if ( "ByMaterials" == type )
		spriteDef = createMaterialSpriteDef( jsonSpriteDef );
	else if ( "Combine" == type )
		spriteDef = createBaseSpriteDef( jsonSpriteDef );
	else if ( "Frames" == type )
		spriteDef = createBaseSpriteDef( jsonSpriteDef );
	else if ( "Rotation" == type )
		spriteDef = createRotationSpriteDef( jsonSpriteDef );
	else if ( "Season" == type )
		spriteDef = createSeasonSpriteDef( jsonSpriteDef );
	else if ( "Random" == type )
		spriteDef = createBaseSpriteDef( jsonSpriteDef );

	return spriteDef;
}


SpriteDefinition* JsonSpriteDefFactory::createCombineSpriteDef( QJsonObject jsonSpriteDef )
{

	QString id                   = jsonSpriteDef.value( "ID" ).toString();
	RotationSpriteDefinition* sp = new RotationSpriteDefinition( id );

	for ( auto rot : m_rotations )
		sp->add( rot, fromJson( jsonSpriteDef.value( rot ).toObject() ) );

	return sp;
}

SpriteDefinition* JsonSpriteDefFactory::createRotationSpriteDef( QJsonObject jsonSpriteDef )
{

	QString id = jsonSpriteDef.value( "ID" ).toString();
	RotationSpriteDefinition* sp = new RotationSpriteDefinition( id );
	
	for ( auto rot : m_rotations )
		sp->add( rot, fromJson( jsonSpriteDef.value( rot ).toObject() ) );

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
		if ( key != "ID" && key != "Type" && key != "Var" && key != "Tint" && key != "Effect" )
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
		if ( key != "ID" && key != "Type" && key != "Var" && key != "Tint" && key != "Effect" )
			sp->add( key, fromJson( jsonSpriteDef.value( key ).toObject() ) );
	}

	return sp;
}

SpriteDefinition* JsonSpriteDefFactory::createBaseSpriteDef( QJsonObject jsonSpriteDef )
{
	QString id               = jsonSpriteDef.value( "ID" ).toString();
	QString tilesheet        = jsonSpriteDef.value( "Tilesheet" ).toString();
	QString sourceRectangle = jsonSpriteDef.value( "SourceRectangle" ).toString();
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
			if ( spriteDef.contains(ID))
				m_jsonDefs.insert( spriteDef.value( ID ).toString(), spriteDef );
			else
				qDebug() << "***ERROR*** no id  for " << spriteDef;
		}
	}

	return true;
}