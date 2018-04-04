#include "quarre-simple-entity.hpp"

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

using namespace score::addons;

quarre::SimpleEntity::SimpleEntity(const Id<SimpleEntity> &id, QObject *parent) :
    Entity(id, "SimpleEntity", parent)
{

}

quarre::SimpleEntity::~SimpleEntity() {}

template <> void DataStreamReader::read(
        const quarre::SimpleEntity& e )
{
    insertDelimiter();

}

template <> void DataStreamWriter::write(
        quarre::SimpleEntity& e )
{
    checkDelimiter();
}


template <> void JSONObjectReader::read(
        const quarre::SimpleEntity& e )
{

}

template <> void JSONObjectWriter::write(
        quarre::SimpleEntity& e )
{

}

