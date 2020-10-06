/*	
	This file is part of Ingnomia https://github.com/rschurade/Ingnomia
    Copyright (C) 2017-2020  Ralph Schurade, Ingnomia Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once

#include "../gfx/sprite.h"

#include <QMap>
#include <QSet>
#include <QPixmap>
#include <QString>

using SDID = QString;


class SpriteDefinition
{
public:
	SpriteDefinition( SDID sID );
	SpriteDefinition( const SpriteDefinition& other );
	virtual ~SpriteDefinition();

	virtual SpriteDefinition* copy()                                                              = 0;
	virtual QPixmap& getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random )    = 0;
	virtual QMap<QString, int> getRandomVariables()                                               = 0;
	virtual int getAnimFrames()                                                                   = 0;
	virtual void getTypes( QSet<QString>* types )                                                 = 0;
	virtual QJsonObject toJson();
	virtual QString toString();


	virtual void replaceVariable( QString replace, QString with );

	SDID m_sID       = "";
	QString m_type   = "";
};

class CachedSpriteDefinition : public SpriteDefinition
{
public:
	CachedSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, bool debug );
	CachedSpriteDefinition( const CachedSpriteDefinition& other );
	~CachedSpriteDefinition();

	SpriteDefinition* copy();
	QPixmap& getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random );
	virtual QMap<QString, int> getRandomVariables();
	virtual int getAnimFrames();
	virtual void getTypes( QSet<QString>* types );
	QJsonObject toJson();

	SpriteDefinition* m_spriteDef;
	QMap<QString, int> m_randomVariables;
	QSet<QString> m_types;
	bool m_debug;
	int m_animFrames = 1;
};

class BaseSpriteDefinition : public SpriteDefinition
{
public:
	BaseSpriteDefinition( SDID sID, QString tilesheet, QString sourcerectangle, QPixmap pm );
	BaseSpriteDefinition( const BaseSpriteDefinition& other );
	~BaseSpriteDefinition();
	
	SpriteDefinition* copy();
	QPixmap& getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random );
	virtual QMap<QString, int> getRandomVariables();
	virtual int getAnimFrames();
	virtual void getTypes( QSet<QString>* types );
	QJsonObject toJson();

	QString m_tilesheet = "";
	QString m_sourcerectangle;
	QString m_offset = "0 0";
	QPixmap m_pixmap;

};

class BranchingSpriteDefinition : public SpriteDefinition
{
public:
	BranchingSpriteDefinition( SDID sID, QString m_variable );
	BranchingSpriteDefinition( const BranchingSpriteDefinition& other );
	~BranchingSpriteDefinition();

	virtual void add( QString key, SpriteDefinition* spriteDef );
	virtual QPixmap& getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random );
	virtual QMap<QString, int> getRandomVariables();
	virtual int getAnimFrames();
	virtual void getTypes( QSet<QString>* types );
	virtual QJsonObject toJson();
	void replaceVariable( QString replace, QString with );

	QString m_variable;
	QMap<QString, SpriteDefinition*> m_sprites;
};

class SeasonSpriteDefinition : public BranchingSpriteDefinition
{
public:
	SeasonSpriteDefinition( SDID sID, QStringList seasons );
	SeasonSpriteDefinition( const SeasonSpriteDefinition& other );
	~SeasonSpriteDefinition();

	SpriteDefinition* copy();

	QJsonObject toJson();

	QList<QString> m_seasons;

};

class RotationSpriteDefinition : public BranchingSpriteDefinition
{
public:
	RotationSpriteDefinition( SDID sID );
	RotationSpriteDefinition( const RotationSpriteDefinition& other );
	~RotationSpriteDefinition();

	SpriteDefinition* copy();
	QJsonObject toJson();
};

class FramesSpriteDefinition : public BranchingSpriteDefinition
{
public:
	FramesSpriteDefinition( SDID sID );
	FramesSpriteDefinition( const FramesSpriteDefinition& other );
	~FramesSpriteDefinition();

	SpriteDefinition* copy();
	virtual int getAnimFrames();
};

class MaterialSpriteDefinition : public BranchingSpriteDefinition
{
public:
	MaterialSpriteDefinition( SDID sID, QString variable );
	MaterialSpriteDefinition( const MaterialSpriteDefinition& other );
	~MaterialSpriteDefinition();

	SpriteDefinition* copy();

};

class TypeSpriteDefinition : public BranchingSpriteDefinition
{
public:
	TypeSpriteDefinition( SDID sID, QString variable, QHash<QString, QString> materialTypes );
	TypeSpriteDefinition( const TypeSpriteDefinition& other );
	~TypeSpriteDefinition();

	SpriteDefinition* copy();
	QPixmap& getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random );

	QHash<QString, QString> m_materialTypes;
};


class CombineSpriteDefinition : public BranchingSpriteDefinition
{
public:
	CombineSpriteDefinition( SDID sID, QStringList seasons );
	CombineSpriteDefinition( const CombineSpriteDefinition& other );
	~CombineSpriteDefinition();

	SpriteDefinition* copy();
	QPixmap& getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random );
	void add( QString key, SpriteDefinition* spriteDef );
	virtual int getAnimFrames();
	QJsonObject toJson();

	QList<QString> m_seasons;
	int m_frames = 0;
};

class RandomSpriteDefinition : public BranchingSpriteDefinition
{
public:
	RandomSpriteDefinition( SDID sID, QString variable );
	RandomSpriteDefinition( const RandomSpriteDefinition& other );
	~RandomSpriteDefinition();

	SpriteDefinition* copy();
	QPixmap& getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random );
	void add( QString key, SpriteDefinition* spriteDef );
	virtual QMap<QString, int> getRandomVariables();
	QJsonObject toJson();

	QList<int> m_weights;
	int m_sum = 0;
};

class LinearSpriteDefinition : public SpriteDefinition
{
public:
	LinearSpriteDefinition( SDID sID, SpriteDefinition* spriteDef );
	LinearSpriteDefinition( const LinearSpriteDefinition& other );
	~LinearSpriteDefinition();

	virtual QMap<QString, int> getRandomVariables();
	virtual int getAnimFrames();
	virtual void getTypes( QSet<QString>* types );
	QJsonObject toJson();

	void replaceVariable( QString replace, QString with );

	SpriteDefinition* m_spriteDef;
};


class TintSpriteDefinition : public LinearSpriteDefinition
{
public:
	TintSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, QString tint );
	TintSpriteDefinition( const TintSpriteDefinition& other );
	~TintSpriteDefinition();

	SpriteDefinition* copy();
	QPixmap& getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random );
	QPixmap& applyTint( QPixmap& pixmap, QString tint );
	QJsonObject toJson();

	void replaceVariable( QString replace, QString with );

	QString m_variable;
};


class EffectSpriteDefinition : public LinearSpriteDefinition
{
public:
	EffectSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, QString effect );
	EffectSpriteDefinition( const EffectSpriteDefinition& other );
	~EffectSpriteDefinition();

	SpriteDefinition* copy();
	QPixmap& getPixmap( QMap<QString, QString> parameters, QMap<QString, int> random );
	QJsonObject toJson();

	QPixmap& applyEffect( QPixmap& pixmap, QString effect );

	QString m_effect;
};
