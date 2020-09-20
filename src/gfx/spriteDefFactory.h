#pragma once


#include "../gfx/sprite.h"
#include "../gfx/spriteDefinition.h"

#include <QBitmap>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QVector>
#include <QSet>

enum Tables
{
	Sprites_ByMaterials,
	Sprites_ByMaterialTypes,
	Sprites_Combine,
	Sprites_Frames,
	Sprites_Random,
	Sprites_Rotations,
	Sprites_Seasons,
	Sprites_Seasons_Rotations
};

class SpriteDefFactory
{
private:
	QHash<QString, QString> m_spriteTable;
	QHash<QString, BaseSpriteDefinition*> m_baseSpriteDefs;
	QHash<QString, SpriteDefinition*> m_spriteDefs;
	QMap<QString, QPixmap> m_tilesheets;
	int m_RandomVarCounter = 0;
	QString m_currentMaterial = "0";

	void scanTable( QString table );
	SpriteDefinition* createSpriteDefinition( QString spriteId );
	BaseSpriteDefinition* createBaseSpriteDefinition( QString spriteId, QVariantMap row );
	SpriteDefinition* createBranchingSpriteDefinition( QString id, QString table );
	BranchingSpriteDefinition* newSpriteDef( QString id, QString table );

protected:

	QHash<QString, QString> m_materialTypes;
	QStringList m_seasons;
	QHash<QString, SpriteDefinition*> m_complexSpriteDefs;

	void loadCaches();
	QPixmap loadTilesheet( QString tilesheet );
	QPixmap extractPixmap( QString id, QPixmap pixmap, QString rect );


public:

	SpriteDefFactory();
	~SpriteDefFactory();


	bool init();
	bool saveToFile( QString filename );
};
