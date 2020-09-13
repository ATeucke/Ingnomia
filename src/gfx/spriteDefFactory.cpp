
#include "spriteDefFactory.h"

#include "../base/config.h"
#include "../base/db.h"

#include <QDebug>
#include <QElapsedTimer>
#include <QHash>
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

SpriteDefFactory::SpriteDefFactory()
{
}

SpriteDefFactory::~SpriteDefFactory()
{
}

bool SpriteDefFactory::init()
{
	m_RandomVarCounter = 0;

	m_seasons.clear();
	m_materialTypes.clear();
	m_spriteTable.clear();
	m_baseSpriteDefs.clear();
	m_spriteDefs.clear();
	m_tilesheets.clear();

	m_seasons = DB::ids( "Seasons" );

	for ( auto id : DB::ids( "Materials" ) )
		m_materialTypes.insert( id, DB::select( "Type", "Materials", id ).toString() );

	for ( auto row : DB::selectRows( "BaseSprites" ) )
	{
		QString id        = row.value( "ID" ).toString();
		QString tilesheet        = row.value( "Tilesheet" ).toString();
		QString sourcerectangle        = row.value( "SourceRectangle" ).toString();
		QPixmap pixmap    = loadTilesheet( tilesheet );
		QPixmap pm        = extractPixmap( pixmap , row);
		BaseSpriteDefinition* sp       = new BaseSpriteDefinition( id, tilesheet, sourcerectangle, pm );
		m_baseSpriteDefs.insert( id, sp );
	}

	QString tables[8] = { "Sprites_ByMaterials", "Sprites_ByMaterialTypes", "Sprites_Combine",
		"Sprites_Frames", "Sprites_Random", "Sprites_Rotations", "Sprites_Seasons", "Sprites_Seasons_Rotations" }; 
	for ( auto table : tables )
		scanTable( table );

	for ( auto sprite : DB::selectRows( "Sprites" ) )
	{
		SpriteDefinition* sd = createSpriteDefinition( sprite.value( "ID" ).toString() );
		//qDebug() << "spritedefinition " << sd->toString();
	}
	saveToFile();
	return true;
}

bool SpriteDefFactory::saveToFile()
{
	QFile saveFile( QStringLiteral( "spriteDefinitions.json" ));
	if ( !saveFile.open( QIODevice::WriteOnly ) )
	{
		qWarning( "Couldn't open save file." );
		return false;
	}
	QJsonArray sprites;
	for ( auto sd : m_spriteDefs )
		sprites.append( sd->toJson() );

	saveFile.write( QJsonDocument( sprites ).toJson( QJsonDocument::Indented ) );
	qDebug() << "spritedefinitions saved as Json to " << saveFile.fileName();

	return true;

}

SpriteDefinition* SpriteDefFactory::createSpriteDefinition( QString spriteId )
{
	if ( m_spriteDefs.contains( spriteId ) )
	{
		SpriteDefinition* spriteDef = m_spriteDefs.value( spriteId )->copy();
		spriteDef->replaceVariable( "0", m_currentMaterial );
		return spriteDef;
	}

	QVariantMap sprite = DB::selectRow( "Sprites", spriteId );

	QString id = spriteId;
	if ( sprite.contains( "BaseSprite" ) && !sprite.value( "BaseSprite" ).toString().isEmpty() )
		id = sprite.value( "BaseSprite" ).toString();

	SpriteDefinition* spriteDef;
	if ( m_baseSpriteDefs.contains(id ))
		spriteDef = createBaseSpriteDefinition( id, sprite );
	else if ( m_spriteTable.contains( id ) )
		spriteDef = createBranchingSpriteDefinition( id, m_spriteTable.value( id ) );
	else
	{
		qDebug() << "***ERROR*** no sprite for " << id;
		return NULL;
	}

	if ( spriteDef && sprite.contains( "Tint" ) && !sprite.value( "Tint" ).toString().isEmpty() )
		spriteDef = new TintSpriteDefinition( spriteDef->m_sID + "_tint", spriteDef, m_currentMaterial );

	m_spriteDefs.insert( spriteId, spriteDef );
	return spriteDef;
}

BaseSpriteDefinition* SpriteDefFactory::createBaseSpriteDefinition( QString spriteId, QVariantMap row )
{
	BaseSpriteDefinition* baseSprite = m_baseSpriteDefs.value( spriteId );
	if ( !baseSprite )
	{
		qDebug() << "***ERROR*** no basesprite for " << spriteId;
		return baseSprite;
	}
	baseSprite = new BaseSpriteDefinition( *baseSprite );

	if ( row.contains( "Offset" ) && !row.value( "Offset" ).isNull() )
	{
		QString offset = row.value( "Offset" ).toString();
		if ( "0 0" != offset && "" != offset )
		{
			baseSprite->m_offset = offset;
		}
	}
	return baseSprite;
}

SpriteDefinition* SpriteDefFactory::createBranchingSpriteDefinition( QString id, QString table )
{
	BranchingSpriteDefinition* sp = newSpriteDef( id, table );
	int i  = 0;
	for ( auto entry : DB::selectRows( table, id ) )
	{
		QString currentMaterial = m_currentMaterial;
		if ( "Sprites_Combine" == table )
			m_currentMaterial = QString::number( i );

		SpriteDefinition* subSprite;
		if ( entry.contains( "BaseSprite" ) && !entry.value( "BaseSprite" ).toString().isEmpty() )
			subSprite = createBaseSpriteDefinition( entry.value( "BaseSprite" ).toString(), entry );
		else if ( entry.contains( "Sprite" ) && !entry.value( "Sprite" ).toString().isEmpty() )
			subSprite = createSpriteDefinition( entry.value( "Sprite" ).toString() ); 
		else /* The Sprites_Seasons_Rotations case */
			subSprite = createBranchingSpriteDefinition( id + entry.value( "Season" ).toString(), "Sprites_Seasons_Rotations" ); 


		if ( !subSprite )
			return subSprite;

		if ( entry.contains( "Effect" ) && !entry.value( "Effect" ).toString().isEmpty() )
		{
			QString effect = entry.value( "Effect" ).toString();
			if ( "none" != effect )
				subSprite = new EffectSpriteDefinition( subSprite->m_sID + effect, subSprite, effect );
		}
		if ( entry.contains( "Tint" ) && !entry.value( "Tint" ).toString().isEmpty() )
		{
			subSprite = new TintSpriteDefinition( subSprite->m_sID + "tint", subSprite, m_currentMaterial );
		}

		if ( "Sprites_ByMaterials" == table )
			sp->add( entry.value( "MaterialID" ).toString(), subSprite );
		else if ( "Sprites_ByMaterialTypes" == table )
			sp->add( entry.value( "MaterialType" ).toString(), subSprite );
		else if ( "Sprites_Combine" == table )
			sp->add( NULL, subSprite );
		else if ( "Sprites_Frames" == table )
			sp->add( "F"+QString::number( i ), subSprite );
		else if ( "Sprites_Rotations" == table )
			sp->add( entry.value( "Rotation" ).toString(), subSprite );
		else if ( "Sprites_Seasons" == table )
			sp->add( entry.value( "Season" ).toString(), subSprite );
		else if ( "Sprites_Seasons_Rotations" == table )
			sp->add( entry.value( "Rotation" ).toString(), subSprite );
		else if ( "Sprites_Random" == table )
			sp->add( entry.value( "Weight" ).toString(), subSprite );

		m_currentMaterial = currentMaterial;
		i++;
	}
	return sp;
}

BranchingSpriteDefinition* SpriteDefFactory::newSpriteDef( QString id, QString table )
{
	if ( "Sprites_ByMaterials" == table )
		return new MaterialSpriteDefinition( id, m_currentMaterial );
	else if ( "Sprites_ByMaterialTypes" == table )
		return new TypeSpriteDefinition( id, m_currentMaterial, m_materialTypes );
	else if ( "Sprites_Combine" == table )
		return new CombineSpriteDefinition( id, m_seasons );
	else if ( "Sprites_Frames" == table )
		return new FramesSpriteDefinition( id );
	else if ( "Sprites_Rotations" == table )
		return new RotationSpriteDefinition( id );
	else if ( "Sprites_Seasons" == table )
		return new SeasonSpriteDefinition( id, m_seasons );
	else if ( "Sprites_Seasons_Rotations" == table )
		return new RotationSpriteDefinition( id );
	else if ( "Sprites_Random" == table )
		return new RandomSpriteDefinition( id, "r" + QString::number( m_RandomVarCounter++ ) );
	return NULL;
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