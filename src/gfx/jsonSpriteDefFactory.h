#pragma once

#include "../gfx/sprite.h"
#include "../gfx/spriteDefinition.h"
#include "../gfx/spriteDefFactory.h"

#include <QBitmap>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QSet>
#include <QVector>


class SpriteDefinitionTemplate
{
public:
	SpriteDefinitionTemplate( QString sID, QJsonObject jsonTemplate, QList<QString> variables );
	SpriteDefinitionTemplate( const SpriteDefinitionTemplate& other );
	~SpriteDefinitionTemplate();

	QString m_sID;
	QList<QString> m_variables;
	QJsonObject m_template;
};

class JsonSpriteDefFactory : public SpriteDefFactory
{
private:

	QHash<QString, QJsonObject> m_jsonDefs;
	QHash<QString, SpriteDefinitionTemplate*> m_templates;
	 

	bool loadSpriteDefinitions();
	void jsonReplace( QJsonValueRef json, QString before, QString after );
	QJsonObject jsonReplace( QJsonObject json, QString before, QString after );
	QJsonArray jsonReplace( QJsonArray json, QString before, QString after );
	SpriteDefinition* fromJson( QJsonObject jsonSpriteDef );

	void createSpriteDefTemplate( QJsonObject jsonSpriteDef ); 

	SpriteDefinition* createApplyTemplateSpriteDef( QJsonObject jsonSpriteDef );

	SpriteDefinition* createRandomSpriteDef( QJsonObject jsonSpriteDef );

	SpriteDefinition* createFramesSpriteDef( QJsonObject jsonSpriteDef );

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
