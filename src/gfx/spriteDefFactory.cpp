
#include "spritefactory.h"

#include "../base/config.h"
#include "../base/db.h"
#include "../base/gamestate.h"
#include "../base/io.h"
#include "../base/util.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPainter>
#include <QPixmap>
#include "spriteDefFactory.h"

SpriteDefFactory::SpriteDefFactory()
{
}

SpriteDefFactory::~SpriteDefFactory()
{
}

bool SpriteDefFactory::init()
{
	m_seasons = DB::ids( "Seasons" );

	auto matIds = DB::ids( "Materials" );
	for ( auto id : matIds )
	{
		m_materialTypes.insert( id, DB::select( "Type", "Materials", id ).toString() );
	}

	bool loaded = true;
	auto rows   = DB::selectRows( "BaseSprites" );
	for ( auto row : rows )
	{
		QString id        = row.value( "ID" ).toString();
		QString tilesheet = row.value( "Tilesheet" ).toString();
		QPixmap pixmap    = loadTilesheet( tilesheet );
		QPixmap pm        = extractPixmap( pixmap , row);
		SpriteDefinition* sp = new BaseSpriteDefinition( id , tilesheet, "0 0", pm );
		m_baseSpriteDefs.insert( id, sp );
	}

	QString tables[8] = { "Sprites_ByMaterials", "Sprites_ByMaterialTypes", "Sprites_Combine",
		"Sprites_Frames", "Sprites_Random", "Sprites_Rotations", "Sprites_Seasons", "Sprites_Seasons_Rotations" }; 
	for ( auto table : tables )
		scanTable( table );

	auto spriteList = DB::selectRows( "Sprites" );
	for ( auto sprite : spriteList )
	{
		QString id = sprite.value( "ID" ).toString();
	}

	return true;
}

SpriteDefinition* SpriteDefFactory::createSpriteDefinition( QString spriteId )
{
	return NULL;
}

SpriteDefinition* SpriteDefFactory::createBranchingSpriteDefinition( QString id, QString table )
{
	auto rows     = DB::selectRows( table, id );
	BranchingSpriteDefinition* sp = newSpriteDef( id, table );
	for ( auto entry : rows )
	{
		int i = 0;
		SpriteDefinition* subSprite;
		if ( entry.contains( "BaseSprite" ) )
			subSprite = m_baseSpriteDefs.value( entry.value( "BaseSprite" ).toString() );
		else
		{
			QString spriteId = entry.value( "Sprite" ).toString();
			subSprite        = createSpriteDefinition( spriteId ); 
		}
		if ( "Sprites_ByMaterials" == table )
			sp->add( entry.value( "MaterialID" ).toString(), subSprite );
		else if ( "Sprites_ByMaterialTypes" == table )
			sp->add( entry.value( "MaterialType" ).toString(), subSprite );
		else if ( "Sprites_Combine" == table )
			sp->add( "", subSprite );
		else if ( "Sprites_Frames" == table )
			sp->add( QString::number( i++ ), subSprite );
		else if ( "Sprites_Rotations" == table )
			sp->add( entry.value( "Rotation" ).toString(), subSprite );
		else if ( "Sprites_Seasons" == table )
			sp->add( entry.value( "Season" ).toString(), subSprite );
		else if ( "Sprites_Seasons_Rotations" == table )
			sp->add( entry.value( "Rotation" ).toString(), subSprite );
		else if ( "Sprites_Random" == table )
			sp->add( entry.value( "Weight" ).toString(), subSprite );

	}

}

BranchingSpriteDefinition* SpriteDefFactory::newSpriteDef( QString id, QString table )
{
	if ( "Sprites_ByMaterials" == table )
		return new MaterialSpriteDefinition( id, "m" + QString::number(m_MatVarCounter++) );
	else if ( "Sprites_ByMaterialTypes" == table )
		return new TypeSpriteDefinition( id, "m" + QString::number( m_MatVarCounter++ ), m_materialTypes );
	else if ( "Sprites_Combine" == table )
		return new CombineSpriteDefinition( id );
	else if ( "Sprites_Frames" == table )
		return new FramesSpriteDefinition( id );
	else if ( "Sprites_Rotations" == table )
		return new RotationSpriteDefinition( id );
	else if ( "Sprites_Seasons" == table )
		return new SeasonSpriteDefinition( id );
	else if ( "Sprites_Seasons_Rotations" == table )
		return new RotationSpriteDefinition( id );
	else if ( "Sprites_Random" == table )
		return new RandomSpriteDefinition( id, "r" + QString::number( m_RandomVarCounter++ ) );

}

void SpriteDefFactory::scanTable( QString table )
{
	auto rows = DB::selectRows( table );
	for ( auto row : rows )
	{
		QString id = row.value( "ID" ).toString();
		m_spriteTable.insert( id, table );
	}
}

QPixmap SpriteDefFactory::loadTilesheet( QString tilesheet )
{
	if ( m_tilesheets.contains( tilesheet ) )
		return m_tilesheets.value( tilesheet );

	QPixmap pm;
	bool loaded = pm.load( Config::getInstance().get( "dataPath" ).toString() + "/tilesheet/" + tilesheet );
	if ( !loaded )
	{
		loaded = pm.load( tilesheet );
		if ( !loaded )
		{
			qDebug() << "SpriteFactory: failed to load " << tilesheet;
			return pm;
		}
	}
	m_tilesheets.insert( tilesheet, pm );
	return pm;
}

QPixmap SpriteDefFactory::extractPixmap( QPixmap pixmap, QVariantMap def )
{
	QString rect = def.value( "SourceRectangle" ).toString();

	QStringList rl = rect.split( " " );
	if ( rl.size() == 4 )
	{
		int x    = rl[0].toInt();
		int y    = rl[1].toInt();
		int dimX = rl[2].toInt();
		int dimY = rl[3].toInt();

		QPixmap p = pixmap.copy( x, y, dimX, dimY );
		p.setMask( p.createMaskFromColor( QColor( 255, 0, 255 ), Qt::MaskInColor ) );

		return p;
	}
	qDebug() << "***ERROR*** extractPixmap() for " << def.value( "ID" ).toString();
	return QPixmap();
}