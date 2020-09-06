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
#include "../gfx/randomness.h"

#include <QMap>
#include <QPixmap>
#include <QString>

using SDID = QString;


class SpriteDefinition
{
public:
	SpriteDefinition( SDID sID );
	SpriteDefinition( const SpriteDefinition& other );
	virtual ~SpriteDefinition();

	virtual Sprite* createSprite( QStringList materialSID, Randomness* random ) = 0;
	virtual Randomness* createRandomness( QStringList materialIDs ) = 0;

	unsigned int uID = 0;
	SDID m_sID       = "";
	QString m_type   = "";
};

class BaseSpriteDefinition : public SpriteDefinition
{
public:
	BaseSpriteDefinition( SDID sID, QString tilesheet, char xOffset = 0, char yOffset = 0 );
	BaseSpriteDefinition( const BaseSpriteDefinition& other );
	~BaseSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random ) ;
	Randomness* createRandomness( QStringList materialIDs );

	char m_xOffset = 0;
	char m_yOffset = 0;
	QString m_tilesheet = "";
	QPixmap m_pixmap;

};

class BranchingSpriteDefinition : public SpriteDefinition
{
public:
	BranchingSpriteDefinition( SDID sID );
	BranchingSpriteDefinition( const BranchingSpriteDefinition& other );
	~BranchingSpriteDefinition();

	virtual Randomness* createRandomness( QStringList materialIDs );

	QMap<QString, SpriteDefinition*> m_sprites;
};

class SeasonSpriteDefinition : public BranchingSpriteDefinition
{
public:
	SeasonSpriteDefinition( SDID sID );
	SeasonSpriteDefinition( const SeasonSpriteDefinition& other );
	~SeasonSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );

};

class RotationSpriteDefinition : public BranchingSpriteDefinition
{
public:
	RotationSpriteDefinition( SDID sID );
	RotationSpriteDefinition( const RotationSpriteDefinition& other );
	~RotationSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );
};

class FramesSpriteDefinition : public BranchingSpriteDefinition
{
public:
	FramesSpriteDefinition( SDID sID );
	FramesSpriteDefinition( const FramesSpriteDefinition& other );
	~FramesSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );
};

class MaterialSpriteDefinition : public BranchingSpriteDefinition
{
public:
	MaterialSpriteDefinition( SDID sID, int m_position );
	MaterialSpriteDefinition( const MaterialSpriteDefinition& other );
	~MaterialSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );
	Randomness* createRandomness( QStringList materialIDs );

	int m_position;
};

class TypeSpriteDefinition : public BranchingSpriteDefinition
{
public:
	TypeSpriteDefinition( SDID sID, int position, QMap<QString, QString> materialTypes );
	TypeSpriteDefinition( const TypeSpriteDefinition& other );
	~TypeSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );
	Randomness* createRandomness( QStringList materialIDs );

	int m_position;
	QMap<QString, QString> m_materialTypes;
};


class CombineSpriteDefinition : public SpriteDefinition
{
public:
	CombineSpriteDefinition( SDID sID );
	CombineSpriteDefinition( const CombineSpriteDefinition& other );
	~CombineSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );
	Randomness* createRandomness( QStringList materialIDs );

	QList<SpriteDefinition*> m_sprites;
};

class RandomSpriteDefinition : public SpriteDefinition
{
public:
	RandomSpriteDefinition( SDID sID );
	RandomSpriteDefinition( const RandomSpriteDefinition& other );
	~RandomSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );
	Randomness* createRandomness( QStringList materialIDs );

	QList<SpriteDefinition*> m_sprites;
	QList<int> m_weights;
	int m_sum =0;
};

class LinearSpriteDefinition : public SpriteDefinition
{
public:
	LinearSpriteDefinition( SDID sID, SpriteDefinition* spriteDef );
	LinearSpriteDefinition( const LinearSpriteDefinition& other );
	~LinearSpriteDefinition();

	virtual Randomness* createRandomness( QStringList materialIDs );

	SpriteDefinition* m_spriteDef;
};


class TintSpriteDefinition : public LinearSpriteDefinition
{
public:
	TintSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, QString tint );
	TintSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, int material );
	TintSpriteDefinition( const TintSpriteDefinition& other );
	~TintSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );

	QString m_tint;
	int m_material = -1;
};


class EffectSpriteDefinition : public LinearSpriteDefinition
{
public:
	EffectSpriteDefinition( SDID sID, SpriteDefinition* spriteDef, QString effect );
	EffectSpriteDefinition( const EffectSpriteDefinition& other );
	~EffectSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );

	QString m_effect;
};
