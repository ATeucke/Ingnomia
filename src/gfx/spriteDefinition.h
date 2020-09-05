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

class MapSpriteDefinition : public SpriteDefinition
{
public:
	MapSpriteDefinition( SDID sID );
	MapSpriteDefinition( const MapSpriteDefinition& other );
	~MapSpriteDefinition();

	virtual Randomness* createRandomness( QStringList materialIDs );

	QMap<QString, SpriteDefinition*> m_sprites;
};

class SeasonSpriteDefinition : public MapSpriteDefinition
{
public:
	SeasonSpriteDefinition( SDID sID );
	SeasonSpriteDefinition( const SeasonSpriteDefinition& other );
	~SeasonSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );

	QMap<QString, SpriteDefinition*> m_seasons;
};

class RotationSpriteDefinition : public MapSpriteDefinition
{
public:
	RotationSpriteDefinition( SDID sID );
	RotationSpriteDefinition( const RotationSpriteDefinition& other );
	~RotationSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );

	QMap<QString, SpriteDefinition*> m_rotations;
};

class FramesSpriteDefinition : public MapSpriteDefinition
{
public:
	FramesSpriteDefinition( SDID sID );
	FramesSpriteDefinition( const FramesSpriteDefinition& other );
	~FramesSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );

	QMap<int, SpriteDefinition*> m_frames;
};


class CombineSpriteDefinition : public MapSpriteDefinition
{
public:
	CombineSpriteDefinition( SDID sID );
	CombineSpriteDefinition( const CombineSpriteDefinition& other );
	~CombineSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );

	QList<SpriteDefinition*> m_sprites;
};

class TypeSpriteDefinition : public MapSpriteDefinition
{
public:
	TypeSpriteDefinition( SDID sID );
	TypeSpriteDefinition( const TypeSpriteDefinition& other );
	~TypeSpriteDefinition();

	Randomness* createRandomness( QStringList materialIDs );

	QMap<QString, SpriteDefinition> m_frames;
};

class MaterialSpriteDefinition : public MapSpriteDefinition
{
public:
	MaterialSpriteDefinition( SDID sID );
	MaterialSpriteDefinition( const MaterialSpriteDefinition& other );
	~MaterialSpriteDefinition();

	Sprite* createSprite( QStringList materialSID, Randomness* random );
	Randomness* createRandomness( QStringList materialIDs );

	QMap<QString, SpriteDefinition> m_frames;
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
	int m_sum;
};

class EffectSpriteDefinition : public SpriteDefinition
{
public:
	EffectSpriteDefinition( SDID sID, SDID spriteDef );
	EffectSpriteDefinition( const EffectSpriteDefinition& other );
	~EffectSpriteDefinition();

	virtual Randomness* createRandomness( QStringList materialIDs );

	SDID m_spriteDef;
};

class TintSpriteDefinition : public EffectSpriteDefinition
{
public:
	TintSpriteDefinition( SDID sID, SDID spriteDef, QString tint );
	TintSpriteDefinition( SDID sID, SDID spriteDef, int material );
	TintSpriteDefinition( const TintSpriteDefinition& other );
	~TintSpriteDefinition();

	QString m_tint;
};

class FlipSpriteDefinition : public EffectSpriteDefinition
{
public:
	FlipSpriteDefinition( SDID sID, SDID spriteDef, QString effect );
	FlipSpriteDefinition( const FlipSpriteDefinition& other );
	~FlipSpriteDefinition();

	QString m_effect;
};

class RotateSpriteDefinition : public EffectSpriteDefinition
{
public:
	RotateSpriteDefinition( SDID sID, SDID spriteDef );
	RotateSpriteDefinition( const RotateSpriteDefinition& other );
	~RotateSpriteDefinition();

	QString m_effect;
	
};