#include "quarre-polymorphic-entity.hpp"

using namespace score::addons;

quarre::PolymorphicEntity::PolymorphicEntity(
        const Id<PolymorphicEntity> &id, QObject *parent) :
    Entity(id, "PolymorphicElement", parent)
{

}

quarre::PolymorphicEntity::~PolymorphicEntity() {}

template <> void DataStreamReader::read(
        const quarre::PolymorphicEntity& e )
{
    insertDelimiter();

}

template <> void DataStreamWriter::write(
        quarre::PolymorphicEntity& e )
{
    checkDelimiter();
}


template <> void JSONObjectReader::read(
        const quarre::PolymorphicEntity& e )
{

}

template <> void JSONObjectWriter::write(
        quarre::PolymorphicEntity& e )
{

}

