#pragma once

#include "../gfx/sprite.h"
#include "../gfx/spriteDefinition.h"
#include "../gfx/spriteDefFactory.h"

#include <QBitmap>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QSet>
#include <QVector>



class JsonSpriteDefFactory : public SpriteDefFactory
{
private:

	QList<QString> m_rotations;
	QHash<QString,QJsonObject> 	m_jsonDefs;
	 

	bool loadSpriteDefinitions();
	SpriteDefinition* fromJson( QJsonObject jsonSpriteDef );

	SpriteDefinition* createCombineSpriteDef( QJsonObject jsonSpriteDef );

	SpriteDefinition* createRotationSpriteDef( QJsonObject jsonSpriteDef );

	SpriteDefinition* createSeasonSpriteDef( QJsonObject jsonSpriteDef );

	SpriteDefinition* createTypeSpriteDef( QJsonObject jsonSpriteDef );

	SpriteDefinition* createMaterialSpriteDef( QJsonObject jsonSpriteDef );

	SpriteDefinition* createBaseSpriteDef( QJsonObject jsonSpriteDef );



public:
	JsonSpriteDefFactory();
	~JsonSpriteDefFactory();

	bool init();
};